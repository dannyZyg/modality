/*
  ==============================================================================

    TransportEngine.h
    Realtime-safe transport and MIDI scheduling engine.

    Design principles:
    - All pattern scheduling happens on the UI thread
    - Audio thread only reads from a lock-free FIFO
    - Each track has independent loop timing (polymetric support)
    - Each track can route to a different MIDI output device

  ==============================================================================
*/

#pragma once

#include "Audio/ScheduledEvent.h"
#include "Data/Note.h"
#include <JuceHeader.h>
#include <array>
#include <atomic>
#include <mutex>

class TransportEngine
{
public:
    static constexpr size_t MAX_TRACKS = 16;
    static constexpr size_t MAX_EVENTS = 4096;
    static constexpr double LOOKAHEAD_BEATS = 2.0;   // Schedule this many beats ahead
    static constexpr double SCHEDULE_THRESHOLD_BEATS = 0.5; // Start scheduling when within this many beats

    TransportEngine();
    ~TransportEngine() = default;

    // === Track Management ===

    /**
     * Set the number of active tracks.
     */
    void setNumTracks (size_t numTracks);

    /**
     * Get the number of active tracks.
     */
    size_t getNumTracks() const;

    // === Per-Track Scheduling (call from UI thread) ===

    /**
     * Schedule a pattern for a specific track with its own timing and output.
     *
     * @param trackIndex The track index (0-based)
     * @param notes The MIDI notes to schedule (already converted from sequence)
     * @param loopStartTime The absolute time when this loop iteration starts
     * @param loopLengthSeconds Duration of this track's loop in seconds
     * @param output MIDI output device for this track (can be nullptr to skip)
     * @param midiChannel MIDI channel for this track (1-16)
     */
    void scheduleTrack (size_t trackIndex,
                        const std::vector<MidiNote>& notes,
                        double loopStartTime,
                        juce::MidiOutput* output,
                        int midiChannel);

    /**
     * Clear all scheduled events. Call when stopping playback.
     */
    void clearScheduledEvents();

    // === Per-Track Timing Queries ===

    /**
     * Check if a specific track needs beat scheduling.
     *
     * @param trackIndex The track index to check
     * @param currentBeat Current transport beat position
     * @return true if this track needs more beats scheduled
     */
    bool trackNeedsBeatScheduling (size_t trackIndex, double currentBeat) const;

    /**
     * Mark beats as scheduled for a track.
     *
     * @param trackIndex The track that was scheduled
     * @param endBeat The highest beat that was scheduled
     */
    void markBeatsScheduled (size_t trackIndex, double endBeat);

    /**
     * Update cached output pointer for a track.
     * Call this before scheduling if the output may have changed.
     */
    void setTrackOutput (size_t trackIndex, juce::MidiOutput* output, int midiChannel);

    // === Transport Control ===

    /**
     * Prepare for playback. Call before starting transport.
     */
    void prepareToPlay (double sampleRate);

    /**
     * Reset all tracks to beginning (time 0).
     * Call when starting playback.
     */
    void reset();

    // === Audio Thread Processing ===

    /**
     * Process MIDI events for the current audio buffer.
     * ONLY call this from the audio thread.
     *
     * This method is realtime-safe: no allocations, no locks.
     * Events are sent to their individual output destinations.
     *
     * @param currentPosition Current transport position in seconds
     * @param bufferDuration Duration of the audio buffer in seconds
     */
    void processBlock (double currentPosition, double bufferDuration, bool isPlaying);

private:
    std::atomic<bool> wasPlaying { false };

    // Sorted event buffer.
    // UI thread writes (protected by eventMutex), audio thread reads only.
    // eventCount is committed atomically after each write so the audio thread
    // always sees a consistent, fully-sorted snapshot.
    std::array<ScheduledEvent, MAX_EVENTS> eventBuffer;
    std::atomic<int> eventCount { 0 }; // number of valid events in eventBuffer
    std::atomic<int> readHead { 0 };   // audio thread's current read position
    std::mutex eventMutex;             // held only by UI thread; never acquired on audio thread

    // Per-track timing state
    std::array<PerTrackState, MAX_TRACKS> trackStates;
    std::atomic<size_t> numActiveTracks { 4 };

    // Timing
    double sampleRate { 44100.0 };

    // Insert a batch of new events into the sorted buffer (call with eventMutex held).
    // Returns false if the buffer would overflow.
    bool insertEventsSorted (const std::vector<ScheduledEvent>& newEvents);
};
