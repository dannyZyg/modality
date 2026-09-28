#pragma once

#include "Audio/Scheduler.h"
#include <JuceHeader.h>

class Transport : public juce::AudioIODeviceCallback
{
public:
    Transport (Scheduler& s);
    ~Transport() override;

    void start();
    void stop();
    bool isPlaying() const;
    double getCurrentPositionSeconds() const;
    void setPosition (double positionSeconds);
    void reset();
    void audioDeviceIOCallbackWithContext (const float* const* inputChannelData,
                                           int numInputChannels,
                                           float* const* outputChannelData,
                                           int numOutputChannels,
                                           int numSamples,
                                           const AudioIODeviceCallbackContext& context) override;

    void audioDeviceAboutToStart (juce::AudioIODevice* device) override;
    void audioDeviceStopped() override;

private:
    Scheduler& scheduler;

    juce::AudioTransportSource transportSource;

    class SilentPositionableSource : public juce::PositionableAudioSource
    {
    public:
        void prepareToPlay (int, double sr) override { sampleRate = sr; }
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

        bool isLooping() const override { return false; }

    private:
        juce::int64 currentPosition = 0;
        double sampleRate = 44100.0;
    };

    std::unique_ptr<SilentPositionableSource> silentSource;
    double sampleRate { 44100.0 };
};
