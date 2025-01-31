#pragma once

#include <JuceHeader.h>
#include "Data/Cursor.h"
#include "Components/SequenceComponent.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::AnimatedAppComponent,
                       public juce::AudioIODeviceCallback, // Handles audio manually
                       public juce::Button::Listener
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

    void buttonClicked(juce::Button* button) override;

    void initialiseMidiClip();

    virtual void audioDeviceIOCallbackWithContext (const float* const* inputChannelData,
                                                   int numInputChannels,
                                                   float* const* outputChannelData,
                                                   int numOutputChannels,
                                                   int numSamples,
                                                   const AudioIODeviceCallbackContext& context) override;

    void audioDeviceStopped() override;
    void audioDeviceAboutToStart(juce::AudioIODevice* device) override;

    // === Struct to Represent MIDI Notes ===
    struct MidiNote
    {
        double time;    // Note start time (in seconds)
        int noteNumber; // MIDI note number (0-127)
        int velocity;   // Velocity (0-127)
        double duration;// Duration in seconds

        MidiNote(double t, int note, int vel, double dur)
            : time(t), noteNumber(note), velocity(vel), duration(dur) {}
    };


private:
    //==============================================================================
    // Your private member variables go here...

    std::unique_ptr<juce::MidiOutput> midiOutput;
    juce::AudioDeviceManager deviceManager;
    juce::String keyText = "";

    Cursor cursor;
    SequenceComponent sequenceComponent;


    juce::AudioTransportSource transportSource;
    std::vector<MidiNote> midiClip;
    double sampleRate = 44100.0;
    double lastProcessedTime = 0;
    const double clipLength = 1.0; // 1 second loop (4 beats @ 120 BPM)

    juce::TextButton startButton, stopButton;

    bool waitingToStart = false;
    double startTime = 0.0;
    const double startDelay = 0.1; // 100ms delay before starting playback

    // Add a custom source that doesn't produce audio
    class SilentPositionableSource : public juce::PositionableAudioSource
    {
    public:
        void prepareToPlay(int, double sampleRate_) override
        {
            sampleRate = sampleRate_;
        }

        void releaseResources() override {}

        void getNextAudioBlock(const juce::AudioSourceChannelInfo& info) override
        {
            info.clearActiveBufferRegion();
            currentPosition += info.numSamples;
        }

        void setNextReadPosition(juce::int64 newPosition) override
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
