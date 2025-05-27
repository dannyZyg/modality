#pragma once

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
#include <queue>

// Add to your class header
struct ScheduledMidiEvent
{
    double timestamp;
    juce::MidiMessage message;
    bool operator> (const ScheduledMidiEvent& other) const
    {
        return timestamp > other.timestamp;
    }
};

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent : public juce::AnimatedAppComponent,
                      public juce::AudioIODeviceCallback // Handles audio manually
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

    virtual void audioDeviceIOCallbackWithContext (const float* const* inputChannelData,
                                                   int numInputChannels,
                                                   float* const* outputChannelData,
                                                   int numOutputChannels,
                                                   int numSamples,
                                                   const AudioIODeviceCallbackContext& context) override;

    void audioDeviceStopped() override;
    void audioDeviceAboutToStart (juce::AudioIODevice* device) override;

private:
    //==============================================================================

    std::unique_ptr<juce::MidiOutput> midiOutput;
    juce::AudioDeviceManager deviceManager;
    juce::String keyText = "";

    Cursor cursor;

    SequenceComponent sequenceComponent;
    CursorComponent cursorComponent;
    MidlineComponent midlineComponent;
    StatusBarComponent statusBarComponent;
    ModifierMenuComponent modifierMenuComponent;
    SequenceSelectionComponent sequenceSelectionComponent;

    ContextualMenuComponent contextualMenuComponent;

    std::unique_ptr<MenuNode> helpMenuRoot;
    std::unique_ptr<MenuNode> globalSettingsMenuRoot;
    std::unique_ptr<MenuNode> sequenceSettingsMenuRoot;
    std::unique_ptr<MenuNode> modifierMenuRoot;

    std::unique_ptr<MenuNode> createHelpMenuTree();

    void showModifierMenu (juce::Point<int> position);
    void hideModifierMenu();

    juce::AudioTransportSource transportSource;
    double sampleRate = 44100.0;
    double lastProcessedTime = 0;
    const double clipLength = 1.0; // 1 second loop (4 beats @ 120 BPM)

    std::priority_queue<ScheduledMidiEvent,
                        std::vector<ScheduledMidiEvent>,
                        std::greater<ScheduledMidiEvent>>
        midiEventQueue;

    double nextPatternStartTime = 0.0;
    const double lookAheadTime = 0.025;
    void scheduleNextPattern (double startTime);

    double midiClipDuration = 2.0; // Duration of one iteration of the clip
    double nextClipStartTime = 0.0; // Track when to add next iteration
    juce::String notesToString (const std::vector<MidiNote>& notes);

    void start();
    void stop();

    void setupKeyboardShortcuts();
    KeyboardShortcutManager shortcutManager;

    // Add a custom source that doesn't produce audio
    class SilentPositionableSource : public juce::PositionableAudioSource
    {
    public:
        void prepareToPlay (int, double sampleRate_) override
        {
            sampleRate = sampleRate_;
        }

        void releaseResources() override {}

        void getNextAudioBlock (const juce::AudioSourceChannelInfo& info) override
        {
            info.clearActiveBufferRegion();
            currentPosition += info.numSamples;
        }

        void setNextReadPosition (juce::int64 newPosition) override
        {
            currentPosition = newPosition;
        }

        juce::int64 getNextReadPosition() const override
        {
            return currentPosition;
        }

        juce::int64 getTotalLength() const override
        {
            return std::numeric_limits<juce::int64>::max();
        }

        bool isLooping() const override
        {
            return false;
        }

    private:
        juce::int64 currentPosition = 0;
        double sampleRate = 44100.0;
    };

    std::unique_ptr<SilentPositionableSource> silentSource;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
