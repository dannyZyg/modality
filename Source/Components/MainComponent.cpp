#include "MainComponent.h"
#include "Components/MidlineComponent.h"
#include "Components/PaginatedSettingsComponent.h"
#include "Components/Widgets/ISelectableWidget.h"
#include "Data/AppSettings.h"
#include "Data/MenuNode.h"
#include "Data/Modifier.h"
#include "Data/Selection.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include <algorithm>
#include <memory>
#include <utility>

//==============================================================================
MainComponent::MainComponent() : sequenceComponent (cursor),
                                 cursorComponent (cursor),
                                 midlineComponent (cursor),
                                 statusBarComponent (cursor),
                                 modifierMenuComponent (cursor),
                                 sequenceSelectionComponent (cursor)
{
    AppSettings::getInstance().initialise ("Modality");

    // Make sure you set the size of the component after
    // you add any child components.
    setSize (AppSettings::getInstance().getLastWindowWidth(), AppSettings::getInstance().getLastWindowHeight());

    setupKeyboardShortcuts();

    setFramesPerSecond (60); // This sets the frequency of the update calls.
    setWantsKeyboardFocus (true);
    addAndMakeVisible (midlineComponent);
    addAndMakeVisible (cursorComponent);
    addAndMakeVisible (sequenceComponent);
    addAndMakeVisible (statusBarComponent);
    addAndMakeVisible (sequenceSelectionComponent);

    addChildComponent (modifierMenuComponent);

    // Initialise audio
    deviceManager.initialise (0, 2, nullptr, true);
    deviceManager.addAudioCallback (this); // Register audio callback

    // Set up MIDI output
    auto midiOutputs = juce::MidiOutput::getAvailableDevices();
    if (! midiOutputs.isEmpty())
    {
        auto deviceInfo = midiOutputs[0];
        midiOutput = juce::MidiOutput::openDevice (deviceInfo.identifier);
        if (midiOutput)
            juce::Logger::writeToLog ("MIDI output initialised: " + deviceInfo.name);
    }

    // Create and set up silent source
    silentSource = std::make_unique<SilentPositionableSource>();
    transportSource.setSource (silentSource.get(), 0, nullptr, sampleRate);

    // Make sure all children components have size set
    resized();

    addAndMakeVisible (contextualMenuComponent);
    contextualMenuComponent.setVisible (false);

    // GLOBAL SETTINGS MENU

    globalSettingsMenuRoot = std::make_unique<MenuNode> ("Settings");

    auto tempoNode = std::make_unique<MenuNode> ("Tempo Settings", juce::KeyPress::createFromDescription ("t"));
    auto deviceNode = std::make_unique<MenuNode> ("Device Settings", juce::KeyPress::createFromDescription ("d"));

    // Add children and receive the raw pointer to them (to further assign children to these) - the original unq ptr has moved!
    MenuNode* tempoNodePtr = globalSettingsMenuRoot->addChild (std::move (tempoNode));
    MenuNode* deviceNodePtr = globalSettingsMenuRoot->addChild (std::move (deviceNode));

    auto deviceSub = std::make_unique<MenuNode> ("Device Sub Menu", juce::KeyPress::createFromDescription ("s"));

    deviceNodePtr->addChild (std::move (deviceSub));

    // Mod Menu
    modifierMenuRoot = std::make_unique<MenuNode> ("Modifiers");

    std::vector<std::unique_ptr<ISelectableWidget>> widgets = createParamWidgets (ModifierType::randomTrigger);
    std::vector<std::unique_ptr<ISelectableWidget>> widgetsVel = createParamWidgets (ModifierType::randomVelocity);
    std::vector<std::unique_ptr<ISelectableWidget>> widgetsOct = createParamWidgets (ModifierType::randomOctaveShift);

    auto randomTriggerModComponent = std::make_unique<PaginatedSettingsComponent> (std::move (widgets));
    auto velocityModComponent = std::make_unique<PaginatedSettingsComponent> (std::move (widgetsVel));
    auto octaveModComponent = std::make_unique<PaginatedSettingsComponent> (std::move (widgetsOct));

    auto randomTriggerModNode = std::make_unique<MenuNode> ("Random Trigger", juce::KeyPress::createFromDescription ("t"), std::move (randomTriggerModComponent));
    auto velocityModNode = std::make_unique<MenuNode> ("Velocity", juce::KeyPress::createFromDescription ("v"), std::move (velocityModComponent));
    auto octaveModNode = std::make_unique<MenuNode> ("Octave", juce::KeyPress::createFromDescription ("o"), std::move (octaveModComponent));

    MenuNode* randomTriggerModNodePtr = modifierMenuRoot->addChild (std::move (randomTriggerModNode));
    MenuNode* velocityModNodePtr = modifierMenuRoot->addChild (std::move (velocityModNode));
    MenuNode* octaveModNodePtr = modifierMenuRoot->addChild (std::move (octaveModNode));
}

MainComponent::~MainComponent()
{
    transportSource.setSource (nullptr);
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
    if (! contextualMenuComponent.isVisible())
    {
        grabKeyboardFocus();
    }
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::whitesmoke);

    // Get the current position from the transport source (in seconds)
    double currentPosition = transportSource.getCurrentPosition();
    sequenceComponent.setCurrentPlayheadTime (currentPosition);
}

void MainComponent::resized()
{
    sequenceComponent.setBounds (50, 50, getWidth() - 100, getHeight() - 100);
    cursorComponent.setBounds (50, 50, getWidth() - 100, getHeight() - 100);
    midlineComponent.setBounds (50, 50, getWidth() - 100, getHeight() - 100);
    sequenceSelectionComponent.setBounds (50, 10, getWidth() - 100, 30);
    statusBarComponent.resized();

    auto menuWidth = getWidth() * 0.6;
    auto menuHeight = getHeight() * 0.8;

    auto xPos = getWidth() / 2 - (menuWidth / 2);
    auto yPos = (getHeight() - menuHeight) / 2;
    juce::Point<int> position = juce::Point<int> (xPos, yPos);

    modifierMenuComponent.setBounds (position.x, position.y, getWidth() * 0.6, 200); // Set your desired size

    contextualMenuComponent.setBounds (position.x, position.y, getWidth() * 0.6, menuHeight);

    AppSettings::getInstance().setLastWindowHeight (getHeight());
    AppSettings::getInstance().setLastWindowWidth (getWidth());

    /* auto bounds = getLocalBounds(); */

    /* // Position modifier menu (center in the window) */
    /* auto menuSize = juce::Rectangle<int>(0, 0, 400, 300); */
    /* menuSize.setCentre(bounds.getCentre()); */

    /* // Initially hide the modifier menu */
    /* modifierMenu->setVisible(false); */
}

void MainComponent::start()
{
    // Stop any currently playing notes
    if (midiOutput)
    {
        for (int channel = 1; channel <= 16; ++channel)
            midiOutput->sendMessageNow (juce::MidiMessage::allNotesOff (channel));
    }

    // Reset everything
    transportSource.setPosition (0.0);
    lastProcessedTime = 0.0;
    nextPatternStartTime = 0.0;

    // Clear the queue-
    midiEventQueue = {}; // Create a new empty queue

    // Schedule initial pattern
    scheduleNextPattern (0.0);

    transportSource.start();
    juce::Logger::writeToLog ("Transport Started");
}

void MainComponent::scheduleNextPattern (double startTime)
{
    // Get fresh pattern from cursor
    auto pattern = cursor.extractMidiSequence (0);

    // Schedule all notes in the pattern
    for (const auto& note : pattern)
    {
        double noteStartTime = startTime + note.startTime;
        double noteEndTime = noteStartTime + note.duration;

        // Schedule note-on
        midiEventQueue.push (ScheduledMidiEvent {
            noteStartTime,
            juce::MidiMessage::noteOn (1, note.noteNumber, (uint8) note.velocity) });

        // Schedule note-off
        midiEventQueue.push (ScheduledMidiEvent {
            noteEndTime,
            juce::MidiMessage::noteOff (1, note.noteNumber) });
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
            midiOutput->sendMessageNow (juce::MidiMessage::allNotesOff (channel));
    }
    juce::Logger::writeToLog ("Transport Stopped");
}

void MainComponent::audioDeviceIOCallbackWithContext ([[maybe_unused]] const float* const* inputChannelData,
                                                      [[maybe_unused]] int numInputChannels,
                                                      float* const* outputChannelData,
                                                      int numOutputChannels,
                                                      int numSamples,
                                                      [[maybe_unused]] const AudioIODeviceCallbackContext& context)
{
    // Clear output buffer
    for (int channel = 0; channel < numOutputChannels; ++channel)
        if (outputChannelData[channel] != nullptr)
            std::fill_n (outputChannelData[channel], numSamples, 0.0f);

    // Update transport
    juce::AudioBuffer<float> tempBuffer (outputChannelData, numOutputChannels, numSamples);
    transportSource.getNextAudioBlock (juce::AudioSourceChannelInfo (tempBuffer));

    if (! transportSource.isPlaying() || ! midiOutput)
        return;

    double currentPosition = transportSource.getCurrentPosition();
    double bufferEndTime = currentPosition + (static_cast<double> (numSamples) / sampleRate);

    // Schedule next pattern if needed
    if (currentPosition >= nextPatternStartTime - lookAheadTime)
    {
        scheduleNextPattern (nextPatternStartTime);
    }

    // Process all events that should occur in this buffer
    while (! midiEventQueue.empty() && midiEventQueue.top().timestamp <= bufferEndTime)
    {
        const auto& event = midiEventQueue.top();
        midiOutput->sendMessageNow (event.message);
        midiEventQueue.pop();
    }

    lastProcessedTime = currentPosition;
}

void MainComponent::audioDeviceAboutToStart (juce::AudioIODevice* device)
{
    juce::Logger::writeToLog ("Audio Device About to Start");
    juce::Logger::writeToLog ("Device name: " + device->getName());
    juce::Logger::writeToLog ("Sample rate: " + juce::String (device->getCurrentSampleRate()));

    sampleRate = static_cast<int> (device->getCurrentSampleRate());
    transportSource.prepareToPlay (512, sampleRate);
}

void MainComponent::audioDeviceStopped()
{
    transportSource.releaseResources();
}

juce::String MainComponent::notesToString (const std::vector<MidiNote>& notes)
{
    juce::String result;
    for (const auto& note : notes)
    {
        result += juce::String (note.noteNumber) + " ";
    }
    return result;
}

bool MainComponent::keyPressed (const juce::KeyPress& key)
{
    if (shortcutManager.handleKeyPress (key, cursor.getMode()))
    {
        return true;
    }

    return false; // Key not handled
}

void MainComponent::showModifierMenu (juce::Point<int> position)
{
    // Set position relative to parent component
    modifierMenuComponent.setBounds (position.x, position.y, getWidth() * 0.6, 200); // Set your desired size
    modifierMenuComponent.setVisible (true);
    //modifierMenuComponent.grabKeyboardFocus();

    if (cursor.findNotesForCursorMode().empty())
    {
        modifierMenuComponent.showMessage ("No notes at cursor");
    }
}

void MainComponent::hideModifierMenu()
{
    modifierMenuComponent.setVisible (false);
    grabKeyboardFocus();
}

void MainComponent::setupKeyboardShortcuts()
{
    std::vector<Shortcut> shortcuts = {
        Shortcut (
            juce::KeyPress (juce::KeyPress::escapeKey),
            { Mode::normal, Mode::insert, Mode::visualBlock, Mode::visualLine },
            [this]()
            {
                if (modifierMenuComponent.isVisible())
                {
                    hideModifierMenu();
                    return true;
                }
                else
                {
                    cursor.enableNormalMode();
                    return true;
                }
            },
            "Exit current menu or return to normal mode"),

        Shortcut (
            juce::KeyPress (juce::KeyPress::escapeKey),
            { Mode::visualLine, Mode::visualBlock },
            [this]()
            {
                cursor.enableNormalMode();
                return true;
            },
            "Return to normal mode"),

        Shortcut (
            juce::KeyPress (juce::KeyPress::spaceKey),
            { Mode::normal, Mode::insert, Mode::visualBlock, Mode::visualLine },
            [this]()
            {
                if (transportSource.isPlaying())
                {
                    stop();
                }
                else
                {
                    start();
                }
                return true;
            },
            "Toggle transport"),

        Shortcut (
            juce::KeyPress (juce::KeyPress::createFromDescription ("i").getKeyCode()),
            { Mode::normal },
            [this]()
            {
                cursor.enableInsertMode();
                return true;
            },
            "Enter insert mode"),

        Shortcut (
            juce::KeyPress (juce::KeyPress::createFromDescription ("h").getKeyCode()),
            { Mode::normal, Mode::insert, Mode::visualBlock, Mode::visualLine },
            [this]()
            {
                cursor.move (Direction::left);
                return true;
            },
            "Move cursor left"),

        Shortcut (
            juce::KeyPress (juce::KeyPress::createFromDescription ("l").getKeyCode()),
            { Mode::normal, Mode::insert, Mode::visualBlock, Mode::visualLine },
            [this]()
            {
                cursor.move (Direction::right);
                return true;
            },
            "Move cursor right"),

        Shortcut (
            juce::KeyPress (juce::KeyPress::createFromDescription ("j").getKeyCode()),
            { Mode::normal, Mode::insert, Mode::visualBlock, Mode::visualLine },
            [this]()
            {
                cursor.move (Direction::down);
                return true;
            },
            "Move cursor down"),

        Shortcut (
            juce::KeyPress (juce::KeyPress::createFromDescription ("k").getKeyCode()),
            { Mode::normal, Mode::insert, Mode::visualBlock, Mode::visualLine },
            [this]()
            {
                cursor.move (Direction::up);
                return true;
            },
            "Move cursor up"),

        Shortcut (
            juce::KeyPress::createFromDescription ("shift+h"),
            { Mode::normal, Mode::visualLine, Mode::visualBlock },
            [this]()
            {
                cursor.moveCursorSelection (Direction::left);
                return true;
            },
            "Move visual selection left"),

        Shortcut (
            juce::KeyPress::createFromDescription ("shift+l"),
            { Mode::normal, Mode::visualLine, Mode::visualBlock },
            [this]()
            {
                cursor.moveCursorSelection (Direction::right);
                return true;
            },
            "Move visual selection right"),

        Shortcut (
            juce::KeyPress::createFromDescription ("shift+j"),
            { Mode::normal, Mode::visualLine, Mode::visualBlock },
            [this]()
            {
                cursor.moveCursorSelection (Direction::down);
                return true;
            },
            "Move visual selection down"),

        Shortcut (
            juce::KeyPress::createFromDescription ("shift+k"),
            { Mode::normal, Mode::visualLine, Mode::visualBlock },
            [this]()
            {
                cursor.moveCursorSelection (Direction::up);
                return true;
            },
            "Move visual selection up"),

        Shortcut (
            juce::KeyPress (juce::KeyPress::createFromDescription ("d").getKeyCode()),
            { Mode::normal, Mode::insert, Mode::visualBlock, Mode::visualLine },
            [this]()
            {
                cursor.decreaseTimelineStepSize();
                return true;
            },
            "Decrease timeline step size"),

        Shortcut (
            juce::KeyPress (juce::KeyPress::createFromDescription ("f").getKeyCode()),
            { Mode::normal, Mode::insert, Mode::visualBlock, Mode::visualLine },
            [this]()
            {
                cursor.increaseTimelineStepSize();
                return true;
            },
            "Increase timeline step size"),

        Shortcut (
            juce::KeyPress ('^', juce::ModifierKeys::shiftModifier, 0),
            { Mode::normal, Mode::insert },
            [this]()
            {
                cursor.jumpToStart();
                return true;
            },
            "Jump cursor to start"),

        Shortcut (
            juce::KeyPress ('$', juce::ModifierKeys::shiftModifier, 0),
            { Mode::normal, Mode::insert },
            [this]()
            {
                cursor.jumpToEnd();
                return true;
            },
            "Jump cursor to end"),

        Shortcut (
            juce::KeyPress (juce::KeyPress::createFromDescription ("w").getKeyCode()),
            { Mode::normal, Mode::insert },
            [this]()
            {
                cursor.jumpForwardBeat();
                return true;
            },
            "Jump cursor forward one beat"),

        Shortcut (
            juce::KeyPress (juce::KeyPress::createFromDescription ("W").getKeyCode()),
            { Mode::normal, Mode::insert },
            [this]()
            {
                cursor.jumpBackBeat();
                return true;
            },
            "Jump cursor back one beat"),

        Shortcut (
            juce::KeyPress (juce::KeyPress::createFromDescription ("v").getKeyCode()),
            { Mode::normal, Mode::insert },
            [this]()
            {
                cursor.enableVisualBlockMode();
                return true;
            },
            "Enable visual block mode"),

        Shortcut (
            juce::KeyPress (juce::KeyPress::createFromDescription ("v").getKeyCode()),
            { Mode::visualBlock, Mode::visualLine },
            [this]()
            {
                cursor.enableNormalMode();
                return true;
            },
            "Turn off visual mode, enable normal mode"),

        Shortcut (
            juce::KeyPress::createFromDescription ("shift+v"),
            { Mode::normal },
            [this]()
            {
                cursor.enableVisualLineMode();
                return true;
            },
            "Enable visual line mode"),

        Shortcut (
            juce::KeyPress::createFromDescription ("shift+v"),
            { Mode::visualBlock, Mode::visualLine },
            [this]()
            {
                cursor.enableNormalMode();
                return true;
            },
            "Turn off visual mode, enable normal mode"),

        Shortcut (
            juce::KeyPress (juce::KeyPress::createFromDescription ("o").getKeyCode()),
            { Mode::visualBlock, Mode::visualLine },
            [this]()
            {
                cursor.cursorPosition = cursor.getVisualSelectionOpposite();
                return true;
            },
            "Jump cursor to opposite visual selection corner"),

        Shortcut (
            juce::KeyPress (juce::KeyPress::returnKey),
            { Mode::insert },
            [this]()
            {
                cursor.insertNote();
                return true;
            },
            "Insert note at cursor position"),

        Shortcut (
            juce::KeyPress (juce::KeyPress::createFromDescription ("x")),
            { Mode::normal, Mode::insert },
            [this]()
            {
                cursor.removeNotesAtCursor();
                return true;
            },
            "Remove note at cursor position"),

        Shortcut (
            juce::KeyPress (juce::KeyPress::createFromDescription ("m")),
            { Mode::normal, Mode::insert, Mode::visualBlock, Mode::visualLine },
            [this]()
            {
                contextualMenuComponent.displayMenu (modifierMenuRoot.get());
                return true;
            },
            "Open modifier menu"),

        Shortcut (
            juce::KeyPress (juce::KeyPress::createFromDescription ("1")),
            { Mode::normal, Mode::insert, Mode::visualBlock, Mode::visualLine },
            [this]()
            {
                cursor.selectSequence (0);
                return true;
            },
            "Select sequence 1"),

        Shortcut (
            juce::KeyPress (juce::KeyPress::createFromDescription ("2")),
            { Mode::normal, Mode::insert, Mode::visualBlock, Mode::visualLine },
            [this]()
            {
                cursor.selectSequence (1);
                return true;
            },
            "Select sequence 2"),

        Shortcut (
            juce::KeyPress (juce::KeyPress::createFromDescription ("3")),
            { Mode::normal, Mode::insert, Mode::visualBlock, Mode::visualLine },
            [this]()
            {
                cursor.selectSequence (2);
                return true;
            },
            "Select sequence 3"),

        Shortcut (
            juce::KeyPress (juce::KeyPress::createFromDescription ("4")),
            { Mode::normal, Mode::insert, Mode::visualBlock, Mode::visualLine },
            [this]()
            {
                cursor.selectSequence (3);
                return true;
            },
            "Select sequence 4"),

        Shortcut (
            juce::KeyPress (juce::KeyPress::createFromDescription ("6")),
            { Mode::normal, Mode::insert, Mode::visualBlock, Mode::visualLine },
            [this]()
            {
                contextualMenuComponent.displayMenu (globalSettingsMenuRoot.get());
                return true;
            },
            "Global Settings"),

    };

    shortcutManager.registerShortcuts (shortcuts);
}
