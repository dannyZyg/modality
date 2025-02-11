#include "MainComponent.h"
#include "juce_core/system/juce_PlatformDefs.h"

//==============================================================================
MainComponent::MainComponent() : sequenceComponent(cursor), cursorComponent(cursor), statusBarComponent(cursor)
{
    // Make sure you set the size of the component after
    // you add any child components.
    setSize (800, 600);
    setFramesPerSecond (60); // This sets the frequency of the update calls.
    setWantsKeyboardFocus(true);
    addAndMakeVisible(cursorComponent);
    addAndMakeVisible(sequenceComponent);
    addAndMakeVisible(statusBarComponent);

    // Initialise audio
    deviceManager.initialise(0, 2, nullptr, true);
    deviceManager.addAudioCallback(this); // Register audio callback

    // Set up MIDI output
    auto midiOutputs = juce::MidiOutput::getAvailableDevices();
    if (!midiOutputs.isEmpty())
    {
        auto deviceInfo = midiOutputs[0];
        midiOutput = juce::MidiOutput::openDevice(deviceInfo.identifier);
        if (midiOutput)
            DBG("MIDI output initialised: " << deviceInfo.name);
    }

    // Create and set up silent source
    silentSource = std::make_unique<SilentPositionableSource>();
    transportSource.setSource(silentSource.get(), 0, nullptr, sampleRate);

    // Make sure all children components have size set
    resized();
}

MainComponent::~MainComponent()
{
    transportSource.setSource(nullptr);
    stop();
}

//==============================================================================
void MainComponent::update()
{
    // This function is called at the frequency specified by the setFramesPerSecond() call
    // in the constructor. You can use it to update counters, animate values, etc.
    sequenceComponent.update();
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    grabKeyboardFocus();
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::whitesmoke);

    g.setColour(juce::Colours::black);
    g.drawLine(0, getHeight()/2, getWidth(), getHeight()/2);
}

void MainComponent::resized()
{
    sequenceComponent.setBounds(50, 50, getWidth() - 100, getHeight() - 100);
    cursorComponent.setBounds(50, 50, getWidth() - 100, getHeight() - 100);
    statusBarComponent.resized();
}

void MainComponent::start()
{
    // Stop any currently playing notes
    if (midiOutput)
    {
        for (int channel = 1; channel <= 16; ++channel)
            midiOutput->sendMessageNow(juce::MidiMessage::allNotesOff(channel));
    }

    // Reset everything
    transportSource.setPosition(0.0);
    lastProcessedTime = 0.0;
    nextPatternStartTime = 0.0;

    // Clear the queue
    midiEventQueue = {}; // Create a new empty queue

    // Schedule initial pattern
    scheduleNextPattern(0.0);

    transportSource.start();
    DBG("Transport Started");
}

void MainComponent::scheduleNextPattern(double startTime)
{
    // Get fresh pattern from cursor
    auto pattern = cursor.extractMidiSequence(0);

    // Schedule all notes in the pattern
    for (const auto& note : pattern)
    {
        double noteStartTime = startTime + note.startTime;
        double noteEndTime = noteStartTime + note.duration;
        DBG("note duration: " << note.duration);

        DBG("start time s: " << noteStartTime);
        DBG("end   time s: " << noteEndTime);


        // Schedule note-on
        midiEventQueue.push(ScheduledMidiEvent{
            noteStartTime,
            juce::MidiMessage::noteOn(1, note.noteNumber, (uint8)note.velocity)
        });

        // Schedule note-off
        midiEventQueue.push(ScheduledMidiEvent{
            noteEndTime,
            juce::MidiMessage::noteOff(1, note.noteNumber)
        });
    }

    nextPatternStartTime = startTime + midiClipDuration;
    DBG("Scheduled pattern starting at: " << startTime
        << " Next pattern at: " << nextPatternStartTime);
}

void MainComponent::stop()
{
    transportSource.stop();

    // Clear the event queue
    midiEventQueue = {};

    // Stop any currently playing notes
    if (midiOutput)
    {
        for (int channel = 1; channel <= 16; ++channel)
            midiOutput->sendMessageNow(juce::MidiMessage::allNotesOff(channel));
    }
    DBG("Transport Stopped");
}

void MainComponent::audioDeviceIOCallbackWithContext(const float* const* inputChannelData,
                                                   int numInputChannels,
                                                   float* const* outputChannelData,
                                                   int numOutputChannels,
                                                   int numSamples,
                                                   const AudioIODeviceCallbackContext& context)
{
    // Clear output buffer
    for (int channel = 0; channel < numOutputChannels; ++channel)
        if (outputChannelData[channel] != nullptr)
            std::fill_n(outputChannelData[channel], numSamples, 0.0f);

    // Update transport
    juce::AudioBuffer<float> tempBuffer(outputChannelData, numOutputChannels, numSamples);
    transportSource.getNextAudioBlock(juce::AudioSourceChannelInfo(tempBuffer));

    if (!transportSource.isPlaying() || !midiOutput)
        return;

    double currentPosition = transportSource.getCurrentPosition();
    double bufferEndTime = currentPosition + (numSamples / sampleRate);

    // Schedule next pattern if needed
    if (currentPosition >= nextPatternStartTime - lookAheadTime)
    {
        scheduleNextPattern(nextPatternStartTime);
    }

    // Process all events that should occur in this buffer
    while (!midiEventQueue.empty() &&
           midiEventQueue.top().timestamp <= bufferEndTime)
    {
        const auto& event = midiEventQueue.top();
        midiOutput->sendMessageNow(event.message);
        midiEventQueue.pop();
    }

    lastProcessedTime = currentPosition;
}

void MainComponent::audioDeviceAboutToStart(juce::AudioIODevice* device)
{
    DBG("Audio Device About to Start");
    DBG("Device name: " << device->getName());
    DBG("Sample rate: " << device->getCurrentSampleRate());

    sampleRate = device->getCurrentSampleRate();
    transportSource.prepareToPlay(512, sampleRate);
    DBG("prepare to play!");
}

void MainComponent::audioDeviceStopped()
{
    transportSource.releaseResources();
}

juce::String MainComponent::notesToString(const std::vector<MidiNote>& notes)
{
    juce::String result;
    for (const auto& note : notes)
    {
        result += juce::String(note.noteNumber) + " ";
    }
    return result;
}

bool MainComponent::keyPressed (const juce::KeyPress& key)
{
    keyText = "Key: " + key.getTextDescription();

    // Handle key presses here
    if (key == juce::KeyPress::spaceKey)
    {
        if (transportSource.isPlaying()) {
            stop();
        } else {
            start();
        }

        return true; // Indicate that the event was handled
    }

    if (key.getTextCharacter() == 'h')
    {
        cursor.moveLeft();
        return true; // Indicate that the event was handled
    }

    if (key.getTextCharacter() == 'l')
    {
        cursor.moveRight();
        return true; // Indicate that the event was handled
    }

    if (key.getTextCharacter() == 'j')
    {
        cursor.moveDown();
        return true;
    }

    if (key.getTextCharacter() == 'k')
    {
        cursor.moveUp();
        return true;
    }

    if (key.getTextCharacter() == 'H')
    {
        if (cursor.isVisualBlockMode() || cursor.isVisualLineMode()) {
            cursor.moveCursorSelection(Direction::left);
        }
        return true;
    }

    if (key.getTextCharacter() == 'L')
    {
        if (cursor.isVisualBlockMode() || cursor.isVisualLineMode()) {
            cursor.moveCursorSelection(Direction::right);
        }
        return true;
    }

    if (key.getTextCharacter() == 'J')
    {
        if (cursor.isVisualBlockMode() || cursor.isVisualLineMode()) {
            cursor.moveCursorSelection(Direction::down);
        }
        return true;
    }

    if (key.getTextCharacter() == 'K')
    {
        if (cursor.isVisualBlockMode() || cursor.isVisualLineMode()) {
            cursor.moveCursorSelection(Direction::up);
        }
        return true;
    }

    if (key.getTextCharacter() == 'd')
    {
        cursor.decreaseTimelineStepSize();
        return true;
    }

    if (key.getTextCharacter() == 'f')
    {
        cursor.increaseTimelineStepSize();
        return true;
    }

    if (key.getTextCharacter() == '^')
    {
        cursor.jumpToStart();
        return true;
    }

    if (key.getTextCharacter() == '$')
    {
        cursor.jumpToEnd();
        return true;
    }

    if (key.getTextCharacter() == 'w')
    {
        cursor.jumpForwardBeat();
        return true;
    }

    if (key.getTextCharacter() == 'W')
    {
        cursor.jumpBackBeat();
        return true;
    }

    if (key == juce::KeyPress::escapeKey)
    {
        cursor.enableNormalMode();
        return true;
    }

    if (key.getTextCharacter() == 'i')
    {
        cursor.enableInsertMode();
        return true;
    }

    if (key.getTextCharacter() == 'v')
    {
        if (cursor.isVisualLineMode() || cursor.isVisualBlockMode()) {
            cursor.enableNormalMode();
        } else {
            cursor.enableVisualBlockMode();
        }
        return true;
    }

    if (key.getTextCharacter() == 'V')
    {
        if (cursor.isVisualLineMode() || cursor.isVisualBlockMode()) {
            cursor.enableNormalMode();
        } else {
            cursor.enableVisualLineMode();
        }
        return true;
    }

    if (key.getTextCharacter() == 'o')
    {
        if (cursor.isVisualBlockMode()) {
            cursor.cursorPosition = cursor.getVisualSelectionOpposite();
        } else if (cursor.isVisualLineMode()) {

            DBG("TOGGLE");
            cursor.toggleLineMode();
            //cursor.cursorPosition = cursor.getVisualSelectionOpposite();
        }
        return true;
    }

    if (key.getTextCharacter() == 'n')
    {
        return true;
    }

    if (key.getTextCharacter() == 'N')
    {
        return true;
    }

    if (key.getTextCharacter() == 's')
    {
        cursor.previewStep();
        return true;
    }

    if (key.getTextCharacter() == '+')
    {
        return true;
    }

    if (key == juce::KeyPress::returnKey)
    {
        if (cursor.isInsertMode()) {
            cursor.insertNote();
            return true;
        }
    }

    if (key.getTextCharacter() == 'x')
    {
        cursor.removeNotesAtCursor();
        return true;
    }

    if (key.getTextCharacter() == 'm')
    {
        cursor.addModifier();
        return true;
    }

    return false; // Pass unhandled keys to the base class
}

