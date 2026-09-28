#pragma once

#include "Audio/MidiOutputManager.h"
#include "Audio/Scheduler.h"
#include "Audio/Transport.h"
#include "Components/BeatLegendComponent.h"
#include "Components/ContextualMenuComponent.h"
#include "Components/CursorComponent.h"
#include "Components/MidlineComponent.h"
#include "Components/Modifiers/ModifierMenuManager.h"
#include "Components/PitchLegendComponent.h"
#include "Components/SequenceComponent.h"
#include "Components/SequenceSelectionComponent.h"
#include "Components/Settings/SequenceSettingsManager.h"
#include "Components/StatusBarComponent.h"
#include "Data/Composition.h"
#include "Data/Cursor.h"
#include "Data/KeyboardShortcutManager.h"
#include "Data/MenuNode.h"
#include <JuceHeader.h>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent : public juce::AnimatedAppComponent
{
public:
    MainComponent();
    ~MainComponent() override;

    void update() override;

    void paint (juce::Graphics& g) override;
    void resized() override;
    bool keyPressed (const juce::KeyPress& key) override;
    void repaintSequenceComponents();
    Composition& getComposition() { return composition; }

private:
    Composition composition;
    Transport transport;
    Scheduler scheduler;
    Cursor cursor;
    MidiOutputManager midiOutputManager;
    juce::AudioDeviceManager deviceManager;

    // UI Components
    SequenceComponent sequenceComponent;
    CursorComponent cursorComponent;
    MidlineComponent midlineComponent;
    PitchLegendComponent pitchLegendComponent;
    BeatLegendComponent beatLegendComponent;
    StatusBarComponent statusBarComponent;
    SequenceSelectionComponent sequenceSelectionComponent;

    ModifierMenuManager modifierMenuManager;

    ContextualMenuComponent contextualMenuComponent;

    // Menu structures
    std::unique_ptr<MenuNode> helpMenuRoot;
    std::unique_ptr<MenuNode> globalSettingsMenuRoot;
    SequenceSettingsManager sequenceSettngsManager;
    KeyboardShortcutManager shortcutManager;
    std::unique_ptr<MenuNode> createHelpMenuTree();

    void scheduleTrackBeats (size_t trackIndex, double currentBeat);
    void checkAndScheduleTracks();

    void start();
    void stop();

    void setupKeyboardShortcuts();
    void findDefaultMidiDevice();
    void setupMenuTree();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
