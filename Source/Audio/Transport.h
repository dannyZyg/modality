/*
  ==============================================================================

    Transport.h
    Unified transport control - tempo, play state, position, and MIDI scheduling.

    Design:
    - Single source of truth for global tempo
    - Wraps JUCE AudioTransportSource for position tracking
    - Owns TransportEngine for MIDI event scheduling
    - Implements AudioIODeviceCallback (audio thread entry point)

  ==============================================================================
*/

#pragma once

#include "Audio/TransportEngine.h"
#include <JuceHeader.h>
#include <atomic>

class Transport : public juce::AudioIODeviceCallback
{
public:
    // Tempo bounds
    static constexpr double MIN_TEMPO = 20.0;
    static constexpr double MAX_TEMPO = 300.0;
    static constexpr double DEFAULT_TEMPO = 120.0;

    Transport();
    ~Transport() override;

    // === Tempo ===

    /**
     * Set the global tempo. If called during playback, the change is
     * queued and applied at each track's next loop boundary.
     *
     * @param bpm Beats per minute (clamped to MIN_TEMPO..MAX_TEMPO)
     */
    void setTempo (double bpm);

    /**
     * Get the current active tempo.
     */
    double getTempo() const;

    /**
     * Check if there's a pending tempo change waiting to be applied.
     */
    bool hasPendingTempoChange() const;

    /**
     * Apply the pending tempo change and return the new tempo.
     * Call this when scheduling a track at its loop boundary.
     * If no change is pending, returns the current tempo.
     */
    double applyPendingTempo();

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
     *
     * @param positionSeconds Position in seconds
     */
    void setPosition (double positionSeconds);

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
                        double loopLengthSeconds,
                        juce::MidiOutput* output,
                        int midiChannel);

    /**
     * Check if a track needs its next loop scheduled.
     */
    bool trackNeedsScheduling (size_t trackIndex) const;

    /**
     * Get the start time of a track's next loop iteration.
     */
    double getTrackNextLoopStartTime (size_t trackIndex) const;

    /**
     * Mark a track's loop as scheduled and advance to next loop.
     */
    void markTrackScheduled (size_t trackIndex, double loopLengthSeconds);

    /**
     * Clear all scheduled MIDI events.
     */
    void clearScheduledEvents();

    /**
     * Reset all tracks to beginning (time 0).
     */
    void reset();

    // === Audio Callback (from AudioIODeviceCallback) ===

    void audioDeviceIOCallbackWithContext (const float* const* inputChannelData,
                                           int numInputChannels,
                                           float* const* outputChannelData,
                                           int numOutputChannels,
                                           int numSamples,
                                           const AudioIODeviceCallbackContext& context) override;

    void audioDeviceAboutToStart (juce::AudioIODevice* device) override;
    void audioDeviceStopped() override;

    // === Utility ===

    /**
     * Convert beats to seconds using current tempo.
     */
    double beatsToSeconds (double beats) const;

    /**
     * Convert seconds to beats using current tempo.
     */
    double secondsToBeats (double seconds) const;

private:
    // Tempo (atomic for thread-safe access)
    std::atomic<double> tempo { DEFAULT_TEMPO };
    std::atomic<double> pendingTempo { DEFAULT_TEMPO };
    std::atomic<bool> tempoPending { false };

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
