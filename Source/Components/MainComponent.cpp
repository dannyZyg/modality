#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent() : sequenceComponent(cursor)
{
    // Make sure you set the size of the component after
    // you add any child components.
    setSize (800, 600);
    setFramesPerSecond (60); // This sets the frequency of the update calls.
    setWantsKeyboardFocus(true);
    addAndMakeVisible(sequenceComponent);

    // Configure Transport UI
    addAndMakeVisible(startButton);
    addAndMakeVisible(stopButton);
    startButton.setButtonText("Start");
    stopButton.setButtonText("Stop");
    startButton.addListener(this);
    stopButton.addListener(this);

    // Initialize audio
    deviceManager.initialise(0, 2, nullptr, true);
    deviceManager.addAudioCallback(this); // Register audio callback

    // Set up MIDI output
    auto midiOutputs = juce::MidiOutput::getAvailableDevices();
    if (!midiOutputs.isEmpty())
    {
        auto deviceInfo = midiOutputs[0];
        midiOutput = juce::MidiOutput::openDevice(deviceInfo.identifier);
        if (midiOutput)
            DBG("MIDI output initialized: " << deviceInfo.name);
    }

    // Create and set up silent source
    silentSource = std::make_unique<SilentPositionableSource>();
    transportSource.setSource(silentSource.get(), 0, nullptr, sampleRate);

    initialiseMidiClip();
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
    g.fillAll (juce::Colours::white);

    g.setFont (20.0f);
    g.setColour(juce::Colours::black);

    g.drawText (keyText, 20, getHeight() - 50, 200, 40, juce::Justification::bottomLeft, true);
    g.drawText (cursor.getModeName(), getWidth() - 250, getHeight() - 50, 200, 40, juce::Justification::bottomRight, true);
    g.drawLine(0, getHeight()/2, getWidth(), getHeight()/2);
}

void MainComponent::resized()
{
    sequenceComponent.setBounds(50, 50, getWidth() - 100, getHeight() - 100);
    startButton.setBounds(10, 10, 100, 30);
    stopButton.setBounds(120, 10, 100, 30);
}

bool MainComponent::keyPressed (const juce::KeyPress& key)
{
    keyText = "Key: " + key.getTextDescription();

    // Handle key presses here
    if (key == juce::KeyPress::spaceKey)
    {
        cursor.previewStep();
        cursor.moveRight();
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

    if (key.getTextCharacter() == 'v')
    {
        cursor.enableVisualMode();
        return true;
    }

    if (key.getTextCharacter() == 'm')
    {
        cursor.enableVisualMode();
        return true;
    }

    if (key == juce::KeyPress::escapeKey)
    {
        cursor.enableNormalMode();
        return true;
    }

    if (key.getTextCharacter() == 'a')
    {
        cursor.addNote();
        return true;
    }

    if (key.getTextCharacter() == 'r')
    {
        cursor.removeNote();
        return true;
    }

    if (key.getTextCharacter() == 'n')
    {
        cursor.nextNoteInStep();
        return true;
    }

    if (key.getTextCharacter() == 'N')
    {
        cursor.prevNoteInStep();
        return true;
    }

    if (key.getTextCharacter() == 's')
    {
        cursor.previewStep();
        return true;
    }

    if (key.getTextCharacter() == 't')
    {
        return true;
    }

    if (key.getTextCharacter() == 'r')
    {
        return true;
    }

    return false; // Pass unhandled keys to the base class
}

void MainComponent::buttonClicked(juce::Button* button)
{
    if (button == &startButton)
    {
        // Stop any currently playing notes
        if (midiOutput)
        {
            for (int channel = 1; channel <= 16; ++channel)
                midiOutput->sendMessageNow(juce::MidiMessage::allNotesOff(channel));
        }

        transportSource.setPosition(0.0);
        lastProcessedTime = 0.0;
        transportSource.start();
        DBG("Transport Started");
    }
    else if (button == &stopButton)
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
}

// === Create a Simple Pattern of 16th Notes ===
void MainComponent::initialiseMidiClip()
{
    midiClip.clear();
    double beatDuration = 60.0 / 120.0 / 4.0; // 16th note at 120 BPM

    for (int i = 0; i < 16; ++i)  // 1 bar of 16th notes
    {
        double time = i * beatDuration;
        midiClip.emplace_back(time, 60 + (i % 4), 100, beatDuration * 0.9); // C4-D4-E4-F4 cycle
    }
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

    double currentPosition = std::fmod(transportSource.getCurrentPosition(), clipLength);

    // Debug output every 100ms or so
    static int debugCounter = 0;
    if (++debugCounter >= 50)
    {
        DBG("Current Position: " << currentPosition);
        debugCounter = 0;
    }

    // Process each note
    for (const auto& note : midiClip)
    {
        // If we've moved past this note's position since last buffer
        if (currentPosition >= note.time && lastProcessedTime < note.time)
        {
            // Note on
            midiOutput->sendMessageNow(juce::MidiMessage::noteOn(1, note.noteNumber, (uint8)note.velocity));
            DBG("Note ON: " << note.noteNumber << " at time: " << currentPosition);

            // Schedule note off
            midiOutput->sendMessageNow(juce::MidiMessage::noteOff(1, note.noteNumber));
        }
    }

    // Handle loop point
    if (currentPosition < lastProcessedTime)
    {
        lastProcessedTime = 0.0;
        DBG("Loop point hit");
    }
    else
    {
        lastProcessedTime = currentPosition;
    }
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
