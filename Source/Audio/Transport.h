/*
  ==============================================================================

    Transport.h
    Transport control - play state, position, and MIDI scheduling.

    Design:
    - Wraps JUCE AudioTransportSource for position tracking
    - Owns TransportEngine for MIDI event scheduling
    - Implements AudioIODeviceCallback (audio thread entry point)
    - Tempo is owned by Composition; Transport has no knowledge of it

  ==============================================================================
*/

#pragma once

#include "Audio/TransportEngine.h"
#include <JuceHeader.h>

class Transport : public juce::AudioIODeviceCallback
{
public:
    Transport();
    ~Transport() override;

    // === Transport Control ===

    /**
     * Start playback from the current position.
     */
    void start();

    /**
     * Stop playback.
     */
    void stop();

    /**
     * Check if transport is currently playing.
     */
    bool isPlaying() const;

    /**
     * Get the current playback position in seconds.
     */
    double getCurrentPosition() const;

    /**
     * Set the playback position.
     */
    void setPosition (double positionSeconds);

    /**
     * Reset all tracks to beginning (time 0).
     */
    void reset();

    /**
     * Reset scheduling state for all tracks without affecting playback position.
     * Call this when tempo changes during playback so tracks reschedule immediately.
     */
    void resetScheduling();

    // === Track Scheduling (delegates to TransportEngine) ===

    /**
     * Set the number of active tracks.
     */
    void setNumTracks (size_t numTracks);

    /**
     * Get the number of active tracks.
     */
    size_t getNumTracks() const;

    /**
     * Schedule a pattern for a specific track.
     */
    void scheduleTrack (size_t trackIndex,
                        const std::vector<MidiNote>& notes,
                        double loopStartTime,
                        juce::MidiOutput* output,
                        int midiChannel);

    /**
     * Check if a track needs beat scheduling.
     */
    bool trackNeedsBeatScheduling (size_t trackIndex, double currentBeat) const;

    /**
     * Mark beats as scheduled for a track.
     */
    void markBeatsScheduled (size_t trackIndex, double endBeat);

    /**
     * Clear all scheduled MIDI events.
     */
    void clearScheduledEvents();

    // === Audio Callback (from AudioIODeviceCallback) ===

    void audioDeviceIOCallbackWithContext (const float* const* inputChannelData,
                                           int numInputChannels,
                                           float* const* outputChannelData,
                                           int numOutputChannels,
                                           int numSamples,
                                           const AudioIODeviceCallbackContext& context) override;

    void audioDeviceAboutToStart (juce::AudioIODevice* device) override;
    void audioDeviceStopped() override;

private:
    // MIDI scheduling engine
    TransportEngine engine;

    // JUCE transport for position tracking
    juce::AudioTransportSource transportSource;

    // Silent audio source (provides position without generating audio)
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
