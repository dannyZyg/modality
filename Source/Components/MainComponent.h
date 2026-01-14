#pragma once

#include "Audio/MidiOutputManager.h"
#include "Audio/Transport.h"
#include "Components/ContextualMenuComponent.h"
#include "Components/CursorComponent.h"
#include "Components/MidlineComponent.h"
#include "Components/ModifierMenuComponent.h"
#include "Components/SequenceComponent.h"
#include "Components/SequenceSelectionComponent.h"
#include "Components/StatusBarComponent.h"
#include "Data/Cursor.h"
#include "Data/KeyboardShortcutManager.h"
#include <JuceHeader.h>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent : public juce::AnimatedAppComponent
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void update() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;

    bool keyPressed (const juce::KeyPress& key) override;

private:
    //==============================================================================

    // Unified transport control (owns tempo, play state, MIDI scheduling)
    Transport transport;

    // MIDI output management (per-track routing)
    MidiOutputManager midiOutputManager;

    // Audio device management
    juce::AudioDeviceManager deviceManager;

    // Data model
    Cursor cursor;

    // UI Components
    SequenceComponent sequenceComponent;
    CursorComponent cursorComponent;
    MidlineComponent midlineComponent;
    StatusBarComponent statusBarComponent;
    ModifierMenuComponent modifierMenuComponent;
    SequenceSelectionComponent sequenceSelectionComponent;

    ContextualMenuComponent contextualMenuComponent;

    // Menu structures
    std::unique_ptr<MenuNode> helpMenuRoot;
    std::unique_ptr<MenuNode> globalSettingsMenuRoot;
    std::unique_ptr<MenuNode> sequenceSettingsMenuRoot;
    std::unique_ptr<MenuNode> modifierMenuRoot;

    std::unique_ptr<MenuNode> createHelpMenuTree();

    void showModifierMenu (juce::Point<int> position);
    void hideModifierMenu();

    // Schedule a single track for its next loop iteration
    void scheduleTrack (size_t trackIndex);

    // Check and schedule any tracks that need their next loop
    void checkAndScheduleTracks();

    void start();
    void stop();

    void setupKeyboardShortcuts();
    KeyboardShortcutManager shortcutManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
