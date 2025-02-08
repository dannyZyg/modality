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
        baseMidiClip = cursor.extractMidiSequence(0);
        return true;
    }

    if (key.getTextCharacter() == 'k')
    {
        cursor.moveUp();
        baseMidiClip = cursor.extractMidiSequence(0);
        return true;
    }

    if (key.getTextCharacter() == 'H')
    {
        if (cursor.isVisualBlockMode() || cursor.isVisualLineMode()) {
            cursor.moveCursorSelection(Direction::left);
            baseMidiClip = cursor.extractMidiSequence(0);
        }
        return true;
    }

    if (key.getTextCharacter() == 'L')
    {
        if (cursor.isVisualBlockMode() || cursor.isVisualLineMode()) {
            cursor.moveCursorSelection(Direction::right);
            baseMidiClip = cursor.extractMidiSequence(0);
        }
        return true;
    }

    if (key.getTextCharacter() == 'J')
    {
        if (cursor.isVisualBlockMode() || cursor.isVisualLineMode()) {
            cursor.moveCursorSelection(Direction::down);
            baseMidiClip = cursor.extractMidiSequence(0);
        }
        return true;
    }

    if (key.getTextCharacter() == 'K')
    {
        if (cursor.isVisualBlockMode() || cursor.isVisualLineMode()) {
            cursor.moveCursorSelection(Direction::up);
            baseMidiClip = cursor.extractMidiSequence(0);
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
        if (cursor.isVisualBlockMode() || cursor.isVisualLineMode()) {
            cursor.cursorPosition = cursor.getVisualSelectionOpposite();
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
    lastProcessedTime = 0.0;  // Important for first note
    nextClipStartTime = midiClipDuration;
    // Reset midiClip to initial state
    baseMidiClip = cursor.extractMidiSequence(0);


    for (auto& m : baseMidiClip) {
        DBG("MIDI at time: " << m.time <<  " , note: " << m.noteNumber);
    }
    midiClip = baseMidiClip;

    transportSource.start();
    DBG("Transport Started");
}

void MainComponent::stop()
{
    transportSource.stop();
    // Stop any currently playing notes
    if (midiOutput)
    {
        for (int channel = 1; channel <= 16; ++channel)
            midiOutput->sendMessageNow(juce::MidiMessage::allNotesOff(channel));
    }
    DBG("Transport Stopped");
}

void MainComponent::extendMidiClip(double currentTime)
{
    // Update the sequence (this will incorporate modifiers?)
    baseMidiClip = cursor.extractMidiSequence(0);

    // If we're getting close to the end of our current notes
    if (currentTime >= nextClipStartTime - 0.5) // Look ahead by 0.5 seconds
    {
        // Add another iteration of the pattern
        std::vector<Sequence::MidiNote> nextIteration;
        for (const auto& note : baseMidiClip)
        {
            // Create a new note with updated timestamp
            nextIteration.emplace_back(
                note.time + nextClipStartTime,
                note.noteNumber,
                note.velocity,
                note.duration
            );
        }

        // Add new notes to midiClip
        midiClip.insert(midiClip.end(), nextIteration.begin(), nextIteration.end());

        // Update next clip start time
        nextClipStartTime += midiClipDuration;

        DBG("Extended clip. Next start time: " << nextClipStartTime);
    }
}

void MainComponent::audioDeviceIOCallbackWithContext(const float* const* inputChannelData,
                                                   int numInputChannels,
                                                   float* const* outputChannelData,
                                                   int numOutputChannels,
                                                   int numSamples,
                                                   const AudioIODeviceCallbackContext& context)
{
    // Clear output buffer (fill output channels with silence)
    for (int channel = 0; channel < numOutputChannels; ++channel)
        if (outputChannelData[channel] != nullptr)
            std::fill_n(outputChannelData[channel], numSamples, 0.0f);

    // Update transport
    juce::AudioBuffer<float> tempBuffer(outputChannelData, numOutputChannels, numSamples);
    transportSource.getNextAudioBlock(juce::AudioSourceChannelInfo(tempBuffer));

    if (!transportSource.isPlaying() || !midiOutput)
        return;

    double currentPosition = transportSource.getCurrentPosition();

    // Extend the MIDI clip if needed
    extendMidiClip(currentPosition);

    // Process each note
    for (const auto& note : midiClip)
    {
        // For notes that should play in this buffer:
        // 1. Regular case: note time is between last and current position
        // 2. First buffer case: note time is less than current position and we haven't processed anything yet
        if ((currentPosition >= note.time && lastProcessedTime < note.time) ||
            (lastProcessedTime == 0.0 && note.time <= currentPosition && note.time < 0.02)) // 20ms threshold
        {
            // Note on
            midiOutput->sendMessageNow(juce::MidiMessage::noteOn(1, note.noteNumber, (uint8)note.velocity));

            // Schedule note off
            midiOutput->sendMessageNow(juce::MidiMessage::noteOff(1, note.noteNumber));
        }
    }

    // Debug output every 100ms or so
    static int debugCounter = 0;
    if (++debugCounter >= 50)
    {
        DBG("Current Position: " << currentPosition << " Notes in clip: " << midiClip.size());
        debugCounter = 0;
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

juce::String MainComponent::notesToString(const std::vector<Sequence::MidiNote>& notes)
{
    juce::String result;
    for (const auto& note : notes)
    {
        result += juce::String(note.noteNumber) + " ";
    }
    return result;
}
