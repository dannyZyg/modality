/*
  ==============================================================================

    TransportEngine.cpp
    Realtime-safe transport and MIDI scheduling engine implementation.

  ==============================================================================
*/

#include "TransportEngine.h"
#include "juce_core/juce_core.h"
#include "juce_core/system/juce_PlatformDefs.h"
#include <algorithm>
#include <iterator>

TransportEngine::TransportEngine()
{
    // Initialize track states with default values
    for (size_t i = 0; i < MAX_TRACKS; ++i)
    {
        trackStates[i].cachedMidiChannel = static_cast<int> (i + 1); // Channels 1-16
    }
}

// === Track Management ===

void TransportEngine::setNumTracks (size_t numTracks)
{
    numActiveTracks.store (std::min (numTracks, MAX_TRACKS));
}

size_t TransportEngine::getNumTracks() const
{
    return numActiveTracks.load();
}

// === Per-Track Scheduling ===

void TransportEngine::scheduleTrack (size_t trackIndex,
                                     const std::vector<MidiNote>& notes,
                                     double loopStartTime,
                                     juce::MidiOutput* output,
                                     int midiChannel)
{
    if (trackIndex >= numActiveTracks.load())
        return;

    if (output == nullptr)
        return; // No output, skip this track

    // Update cached state for this track
    auto& state = trackStates[trackIndex];
    state.cachedOutput = output;
    state.cachedMidiChannel = midiChannel;

    // Build the new events for this track (UI thread - allocation is safe here)
    std::vector<ScheduledEvent> newEvents;
    newEvents.reserve (notes.size() * 2);

    for (const auto& note : notes)
    {
        double noteStartTime = loopStartTime + note.startTime;
        double noteEndTime = noteStartTime + note.duration;

        newEvents.push_back (ScheduledEvent {
            noteStartTime,
            juce::MidiMessage::noteOn (midiChannel, note.noteNumber, static_cast<juce::uint8> (note.velocity)),
            output });

        newEvents.push_back (ScheduledEvent {
            noteEndTime,
            juce::MidiMessage::noteOff (midiChannel, note.noteNumber),
            output });
    }

    insertEventsSorted (newEvents);
}

bool TransportEngine::insertEventsSorted (const std::vector<ScheduledEvent>& newEvents)
{
    if (newEvents.empty())
        return true;

    std::lock_guard<std::mutex> lock (eventMutex);

    int currentCount = eventCount.load();
    int head = readHead.load();

    // Compact: discard already-consumed events from the front of the buffer so
    // we don't run out of space over time.
    if (head > 0)
    {
        int remaining = currentCount - head;
        if (remaining > 0)
            std::move (eventBuffer.begin() + head,
                       eventBuffer.begin() + currentCount,
                       eventBuffer.begin());
        currentCount = remaining;
        readHead.store (0);
        eventCount.store (currentCount);
        head = 0;
    }

    // Overflow guard
    if (currentCount + static_cast<int> (newEvents.size()) > static_cast<int> (MAX_EVENTS))
    {
        juce::Logger::writeToLog ("TransportEngine: event buffer overflow, events dropped");
        return false;
    }

    // Append new events then stable-sort the entire live region so the buffer
    // stays globally ordered by timestamp.
    std::copy (newEvents.begin(), newEvents.end(), eventBuffer.begin() + currentCount);
    int newCount = currentCount + static_cast<int> (newEvents.size());
    std::stable_sort (eventBuffer.begin(), eventBuffer.begin() + newCount, [] (const ScheduledEvent& a, const ScheduledEvent& b)
                      { return a.timestamp < b.timestamp; });

    // Commit the new count atomically so the audio thread sees a consistent snapshot.
    eventCount.store (newCount);
    return true;
}

void TransportEngine::clearScheduledEvents()
{
    // Reset atomically — no mutex needed because:
    // - Setting eventCount to 0 first causes the audio thread to stop reading immediately.
    // - Setting readHead to 0 afterwards is safe because the audio thread will see
    //   eventCount == 0 and never dereference readHead until new events are committed.
    // UI-thread callers that also hold eventMutex (insertEventsSorted) are safe because
    // they re-read eventCount and readHead after acquiring the lock.
    eventCount.store (0, std::memory_order_release);
    readHead.store (0, std::memory_order_release);
}

// === Per-Track Timing Queries ===

bool TransportEngine::trackNeedsBeatScheduling (size_t trackIndex, double currentBeat) const
{
    if (trackIndex >= numActiveTracks.load())
        return false;

    double lastScheduled = trackStates[trackIndex].lastScheduledBeat.load();
    return currentBeat >= (lastScheduled - TransportEngine::SCHEDULE_THRESHOLD_BEATS);
}

void TransportEngine::markBeatsScheduled (size_t trackIndex, double endBeat)
{
    if (trackIndex >= MAX_TRACKS)
        return;

    trackStates[trackIndex].lastScheduledBeat.store (endBeat);
}

void TransportEngine::setTrackOutput (size_t trackIndex, juce::MidiOutput* output, int midiChannel)
{
    if (trackIndex >= MAX_TRACKS)
        return;

    trackStates[trackIndex].cachedOutput = output;
    trackStates[trackIndex].cachedMidiChannel = midiChannel;
}

// === Transport Control ===

void TransportEngine::prepareToPlay (double newSampleRate)
{
    sampleRate = newSampleRate;
}

void TransportEngine::reset()
{
    // Clear any pending events
    clearScheduledEvents();

    // Reset all track timing to start from 0
    for (size_t i = 0; i < MAX_TRACKS; ++i)
    {
        trackStates[i].reset();
    }
}

// === Audio Thread Processing ===

void TransportEngine::processBlock (double currentPosition, double bufferDuration, bool isPlaying)
{
    // check if transport has just stopped
    if (wasPlaying.load() && ! isPlaying)
    {
        for (size_t i = 0; i < numActiveTracks.load(); ++i)
        {
            auto& trackState = trackStates[i];
            if (trackState.cachedOutput != nullptr)
            {
                juce::Logger::writeToLog ("Sending All Notes Off on channel " + juce::String (trackState.cachedMidiChannel));
                // send note off messages to active channels
                trackState.cachedOutput->sendMessageNow (juce::MidiMessage::allNotesOff (trackState.cachedMidiChannel));
                trackState.cachedOutput->sendMessageNow (juce::MidiMessage::allSoundOff (trackState.cachedMidiChannel));
            }
        }
        clearScheduledEvents();
    }

    wasPlaying.store (isPlaying);

    if (! isPlaying)
        return;

    double bufferEndTime = currentPosition + bufferDuration;

    // Walk the sorted event buffer from readHead, firing every event whose
    // timestamp falls within this buffer's time window.  Because the buffer is
    // globally sorted by timestamp, we can stop at the first future event —
    // everything after it is also in the future.
    int head = readHead.load (std::memory_order_acquire);
    int count = eventCount.load (std::memory_order_acquire);

    while (head < count && eventBuffer[static_cast<size_t> (head)].timestamp <= bufferEndTime)
    {
        const auto& event = eventBuffer[static_cast<size_t> (head)];
        if (event.output != nullptr)
            event.output->sendMessageNow (event.message);
        ++head;
    }

    readHead.store (head, std::memory_order_release);
}
