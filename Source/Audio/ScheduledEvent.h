/*
  ==============================================================================

    ScheduledEvent.h
    Realtime-safe MIDI event structure for lock-free audio processing.

    Each event contains its own output destination, enabling per-track
    MIDI routing in a polymetric environment.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

/**
 * A MIDI event with timestamp and output destination.
 * Designed for use in lock-free data structures between UI and audio threads.
 *
 * The output pointer is cached during scheduling (UI thread) and read
 * during processing (audio thread). The pointer must remain valid for
 * the duration of playback.
 */
struct ScheduledEvent
{
    double timestamp = 0.0;
    juce::MidiMessage message;
    juce::MidiOutput* output = nullptr; // Per-event output destination

    ScheduledEvent() = default;

    ScheduledEvent (double time, juce::MidiMessage msg, juce::MidiOutput* out = nullptr)
        : timestamp (time), message (std::move (msg)), output (out)
    {
    }

    bool operator> (const ScheduledEvent& other) const
    {
        return timestamp > other.timestamp;
    }

    bool operator< (const ScheduledEvent& other) const
    {
        return timestamp < other.timestamp;
    }
};

/**
 * Runtime state for a single track's loop timing.
 * Used by TransportEngine to track independent loop positions per track.
 */
struct PerTrackState
{
    std::atomic<double> nextLoopStartTime { 0.0 };
    std::atomic<double> loopLengthSeconds { 2.0 };

    // Cached for realtime access - set during scheduling, read during processing
    juce::MidiOutput* cachedOutput = nullptr;
    int cachedMidiChannel = 1;

    PerTrackState() = default;

    // Reset to initial state
    void reset()
    {
        nextLoopStartTime.store (0.0);
        // Keep loopLengthSeconds as-is (will be updated from sequence)
    }
};
