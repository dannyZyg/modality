#include "MainComponent.h"
#include "Data/AppSettings.h"

//==============================================================================
MainComponent::MainComponent() : sequenceComponent(cursor),
                                 cursorComponent(cursor),
                                 statusBarComponent(cursor),
                                 modifierMenuComponent(cursor)
{
    AppSettings::getInstance().initialise("Modality");

    // Make sure you set the size of the component after
    // you add any child components.
    setSize (AppSettings::getInstance().getLastWindowWidth(), AppSettings::getInstance().getLastWindowHeight());

    setupKeyboardShortcuts();

    setFramesPerSecond (60); // This sets the frequency of the update calls.
    setWantsKeyboardFocus(true);
    addAndMakeVisible(cursorComponent);
    addAndMakeVisible(sequenceComponent);
    addAndMakeVisible(statusBarComponent);

    addChildComponent(modifierMenuComponent);

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
            juce::Logger::writeToLog("MIDI output initialised: " + deviceInfo.name);
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

    AppSettings::getInstance().shutdown();
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

    if (!modifierMenuComponent.isVisible()) {
        grabKeyboardFocus();
    }
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::whitesmoke);

}

void MainComponent::resized()
{
    sequenceComponent.setBounds(50, 50, getWidth() - 100, getHeight() - 100);
    cursorComponent.setBounds(50, 50, getWidth() - 100, getHeight() - 100);
    statusBarComponent.resized();

    AppSettings::getInstance().setLastWindowHeight(getHeight());
    AppSettings::getInstance().setLastWindowWidth(getWidth());
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

    // Clear the queue-
    midiEventQueue = {}; // Create a new empty queue

    // Schedule initial pattern
    scheduleNextPattern(0.0);

    transportSource.start();
    juce::Logger::writeToLog("Transport Started");
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
    juce::Logger::writeToLog("Transport Stopped");
}

void MainComponent::audioDeviceIOCallbackWithContext([[maybe_unused]] const float* const* inputChannelData,
                                                   [[maybe_unused]] int numInputChannels,
                                                   float* const* outputChannelData,
                                                   int numOutputChannels,
                                                   int numSamples,
                                                   [[maybe_unused]] const AudioIODeviceCallbackContext& context)
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
    juce::Logger::writeToLog("Audio Device About to Start");
    juce::Logger::writeToLog("Device name: " + device->getName());
    juce::Logger::writeToLog("Sample rate: " + juce::String(device->getCurrentSampleRate()));

    sampleRate = device->getCurrentSampleRate();
    transportSource.prepareToPlay(512, sampleRate);
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

bool MainComponent::keyPressed(const juce::KeyPress& key)
{
    if (shortcutManager.handleKeyPress(key, cursor.getMode()))
    {
        return true;
    }

    return false; // Key not handled
}

void MainComponent::showModifierMenu(juce::Point<int> position)
{
    // Set position relative to parent component
    modifierMenuComponent.setBounds(position.x, position.y, getWidth() * 0.6, 200); // Set your desired size
    modifierMenuComponent.setVisible(true);
    modifierMenuComponent.grabKeyboardFocus();

    if (cursor.findNotesForCursorMode().empty()) {
        modifierMenuComponent.showMessage("No notes at cursor");
    }
}

void MainComponent::hideModifierMenu()
{
    modifierMenuComponent.setVisible(false);
    grabKeyboardFocus();
}

void MainComponent::setupKeyboardShortcuts()
{
    shortcutManager.addShortcut(
        juce::KeyPress(juce::KeyPress::escapeKey),
        Mode::insert,
        [this]() {
            if (modifierMenuComponent.isVisible()) {
                hideModifierMenu();
                return true;
            } else {
                cursor.enableNormalMode();
                return true;
            }
        },
        "Exit current menu or return to normal mode"
    );

    shortcutManager.addShortcut(
        juce::KeyPress(juce::KeyPress::escapeKey),
        Mode::visualLine,
        [this]() {
            cursor.enableNormalMode();
            return true;
        },
        "Return to normal mode"
    );

    shortcutManager.addShortcut(
        juce::KeyPress(juce::KeyPress::escapeKey),
        Mode::visualBlock,
        [this]() {
            cursor.enableNormalMode();
            return true;
        },
        "Return to normal mode"
    );

    shortcutManager.addShortcut(
        juce::KeyPress(juce::KeyPress::spaceKey),
        Mode::normal,
        [this]() {
            if (transportSource.isPlaying()) {
                stop();
            } else {
                start();
            }
            return true;
        },
        "Toggle transport"
    );

    shortcutManager.addShortcut(
        juce::KeyPress(juce::KeyPress::createFromDescription("i").getKeyCode()),
        Mode::normal,
        [this]() {
            cursor.enableInsertMode();
            return true;
        },
        "Enter insert mode"
    );

    shortcutManager.addShortcut(
        juce::KeyPress(juce::KeyPress::createFromDescription("h").getKeyCode()),
        Mode::normal,
        [this]() {
            cursor.moveLeft();
            return true;
        },
        "Move cursor left"
    );

    shortcutManager.addShortcut(
        juce::KeyPress(juce::KeyPress::createFromDescription("h").getKeyCode()),
        Mode::visualBlock,
        [this]() {
            cursor.moveLeft();
            return true;
        },
        "Move cursor left"
    );

    shortcutManager.addShortcut(
        juce::KeyPress(juce::KeyPress::createFromDescription("h").getKeyCode()),
        Mode::visualLine,
        [this]() {
            cursor.moveLeft();
            return true;
        },
        "Move cursor left"
    );

    shortcutManager.addShortcut(
        juce::KeyPress(juce::KeyPress::createFromDescription("l").getKeyCode()),
        Mode::normal,
        [this]() {
            cursor.moveRight();
            return true;
        },
        "Move cursor right"
    );

    shortcutManager.addShortcut(
        juce::KeyPress(juce::KeyPress::createFromDescription("l").getKeyCode()),
        Mode::visualLine,
        [this]() {
            cursor.moveRight();
            return true;
        },
        "Move cursor right"
    );

    shortcutManager.addShortcut(
        juce::KeyPress(juce::KeyPress::createFromDescription("l").getKeyCode()),
        Mode::visualBlock,
        [this]() {
            cursor.moveRight();
            return true;
        },
        "Move cursor right"
    );

    shortcutManager.addShortcut(
        juce::KeyPress(juce::KeyPress::createFromDescription("j").getKeyCode()),
        Mode::normal,
        [this]() {
            cursor.moveDown();
            return true;
        },
        "Move cursor down"
    );

    shortcutManager.addShortcut(
        juce::KeyPress(juce::KeyPress::createFromDescription("j").getKeyCode()),
        Mode::visualLine,
        [this]() {
            cursor.moveDown();
            return true;
        },
        "Move cursor down"
    );

    shortcutManager.addShortcut(
        juce::KeyPress(juce::KeyPress::createFromDescription("j").getKeyCode()),
        Mode::visualBlock,
        [this]() {
            cursor.moveDown();
            return true;
        },
        "Move cursor down"
    );

    shortcutManager.addShortcut(
        juce::KeyPress(juce::KeyPress::createFromDescription("k").getKeyCode()),
        Mode::normal,
        [this]() {
            cursor.moveUp();
            return true;
        },
        "Move cursor up"
    );

    shortcutManager.addShortcut(
        juce::KeyPress(juce::KeyPress::createFromDescription("k").getKeyCode()),
        Mode::visualLine,
        [this]() {
            cursor.moveUp();
            return true;
        },
        "Move cursor up"
    );

    shortcutManager.addShortcut(
        juce::KeyPress(juce::KeyPress::createFromDescription("k").getKeyCode()),
        Mode::visualBlock,
        [this]() {
            cursor.moveUp();
            return true;
        },
        "Move cursor up"
    );

    shortcutManager.addShortcut(
        juce::KeyPress(juce::KeyPress::createFromDescription("H").getKeyCode()),
        Mode::visualLine,
        [this]() {
            cursor.moveCursorSelection(Direction::left);
            return true;
        },
        "Move visual line selection left"
    );

    shortcutManager.addShortcut(
        juce::KeyPress(juce::KeyPress::createFromDescription("H").getKeyCode()),
        Mode::visualBlock,
        [this]() {
            cursor.moveCursorSelection(Direction::left);
            return true;
        },
        "Move visual block selection left"
    );

    shortcutManager.addShortcut(
        juce::KeyPress(juce::KeyPress::createFromDescription("L").getKeyCode()),
        Mode::visualLine,
        [this]() {
            cursor.moveCursorSelection(Direction::right);
            return true;
        },
        "Move visual line selection right"
    );

    shortcutManager.addShortcut(
        juce::KeyPress(juce::KeyPress::createFromDescription("L").getKeyCode()),
        Mode::visualBlock,
        [this]() {
            cursor.moveCursorSelection(Direction::right);
            return true;
        },
        "Move visual block selection right"
    );

    shortcutManager.addShortcut(
        juce::KeyPress(juce::KeyPress::createFromDescription("J").getKeyCode()),
        Mode::visualLine,
        [this]() {
            cursor.moveCursorSelection(Direction::down);
            return true;
        },
        "Move visual line selection down"
    );

    shortcutManager.addShortcut(
        juce::KeyPress(juce::KeyPress::createFromDescription("J").getKeyCode()),
        Mode::visualBlock,
        [this]() {
            cursor.moveCursorSelection(Direction::down);
            return true;
        },
        "Move visual block selection down"
    );

    shortcutManager.addShortcut(
        juce::KeyPress(juce::KeyPress::createFromDescription("K").getKeyCode()),
        Mode::visualLine,
        [this]() {
            cursor.moveCursorSelection(Direction::up);
            return true;
        },
        "Move visual line selection up"
    );

    shortcutManager.addShortcut(
        juce::KeyPress(juce::KeyPress::createFromDescription("K").getKeyCode()),
        Mode::visualBlock,
        [this]() {
            cursor.moveCursorSelection(Direction::up);
            return true;
        },
        "Move visual block selection up"
    );

    shortcutManager.addShortcut(
        juce::KeyPress(juce::KeyPress::createFromDescription("d").getKeyCode()),
        Mode::normal,
        [this]() {
            cursor.decreaseTimelineStepSize();
            return true;
        },
        "Decrease timeline step size"
    );

    shortcutManager.addShortcut(
        juce::KeyPress(juce::KeyPress::createFromDescription("f").getKeyCode()),
        Mode::normal,
        [this]() {
            cursor.increaseTimelineStepSize();
            return true;
        },
        "Increase timeline step size"
    );

    shortcutManager.addShortcut(
        juce::KeyPress('^', juce::ModifierKeys::shiftModifier, 0),
        Mode::normal,
        [this]() {
            cursor.jumpToStart();
            return true;
        },
        "Jump cursor to start"
    );

    shortcutManager.addShortcut(
        juce::KeyPress('$', juce::ModifierKeys::shiftModifier, 0),
        Mode::normal,
        [this]() {
            cursor.jumpToEnd();
            return true;
        },
        "Jump cursor to end"
    );

    shortcutManager.addShortcut(
        juce::KeyPress(juce::KeyPress::createFromDescription("w").getKeyCode()),
        Mode::normal,
        [this]() {
            cursor.jumpForwardBeat();
            return true;
        },
        "Jump cursor forward one beat"
    );

    shortcutManager.addShortcut(
        juce::KeyPress(juce::KeyPress::createFromDescription("W").getKeyCode()),
        Mode::normal,
        [this]() {
            cursor.jumpBackBeat();
            return true;
        },
        "Jump cursor back one beat"
    );

    shortcutManager.addShortcut(
        juce::KeyPress(juce::KeyPress::createFromDescription("v").getKeyCode()),
        Mode::normal,
        [this]() {
            cursor.enableVisualBlockMode();
            return true;
        },
        "Enable visual block mode"
    );

    shortcutManager.addShortcut(
        juce::KeyPress(juce::KeyPress::createFromDescription("v").getKeyCode()),
        Mode::visualBlock,
        [this]() {
            cursor.enableNormalMode();
            return true;
        },
        "Turn off visual block mode, enable normal mode"
    );

    shortcutManager.addShortcut(
        juce::KeyPress(juce::KeyPress::createFromDescription("v").getKeyCode()),
        Mode::visualLine,
        [this]() {
            cursor.enableNormalMode();
            return true;
        },
        "Turn off visual line mode, enable normal mode"
    );

    shortcutManager.addShortcut(
        juce::KeyPress::createFromDescription("shift+v"),
        Mode::normal,
        [this]() {
            cursor.enableVisualLineMode();
            return true;
        },
        "Enable visual line mode"
    );

    shortcutManager.addShortcut(
        juce::KeyPress::createFromDescription("shift+v"),
        Mode::visualBlock,
        [this]() {
            cursor.enableNormalMode();
            return true;
        },
        "Turn off visual block mode, enable normal mode"
    );

    shortcutManager.addShortcut(
        juce::KeyPress::createFromDescription("shift+v"),
        Mode::visualLine,
        [this]() {
            cursor.enableNormalMode();
            return true;
        },
        "Turn off visual line mode, enable normal mode"
    );

    shortcutManager.addShortcut(
        juce::KeyPress(juce::KeyPress::createFromDescription("o").getKeyCode()),
        Mode::visualLine,
        [this]() {
            cursor.cursorPosition = cursor.getVisualSelectionOpposite();
            return true;
        },
        "Jump cursor to opposite selection corner"
    );

    shortcutManager.addShortcut(
        juce::KeyPress(juce::KeyPress::createFromDescription("o").getKeyCode()),
        Mode::visualBlock,
        [this]() {
            cursor.cursorPosition = cursor.getVisualSelectionOpposite();
            return true;
        },
        "Jump cursor to opposite selection corner"
    );

    shortcutManager.addShortcut(
        juce::KeyPress(juce::KeyPress::returnKey),
        Mode::insert,
        [this]() {
            cursor.insertNote();
            return true;
        },
        "Insert note at cursor position"
    );

    shortcutManager.addShortcut(
        juce::KeyPress(juce::KeyPress::createFromDescription("x")),
        Mode::normal,
        [this]() {
            cursor.removeNotesAtCursor();
            return true;
        },
        "Remove note at cursor position"
    );

    shortcutManager.addShortcut(
        juce::KeyPress(juce::KeyPress::createFromDescription("x")),
        Mode::insert,
        [this]() {
            cursor.removeNotesAtCursor();
            return true;
        },
        "Remove note at cursor position"
    );

    shortcutManager.addShortcut(
        juce::KeyPress(juce::KeyPress::createFromDescription("m")),
        Mode::insert,
        [this]() {
            auto menuWidth = getWidth() * 0.6;
            showModifierMenu(juce::Point<int>(getWidth() / 2 - (menuWidth/2), getHeight() / 2));
            return true;
        },
        "Open modifier menu"
    );
}
