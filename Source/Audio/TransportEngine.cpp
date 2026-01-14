/*
  ==============================================================================

    TransportEngine.cpp
    Realtime-safe transport and MIDI scheduling engine implementation.

  ==============================================================================
*/

#include "TransportEngine.h"
#include <algorithm>

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
                                      double loopLengthSeconds,
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
    state.loopLengthSeconds.store (loopLengthSeconds);

    // Schedule all notes in the pattern
    for (const auto& note : notes)
    {
        double noteStartTime = loopStartTime + note.startTime;
        double noteEndTime = noteStartTime + note.duration;

        // Schedule note-on
        writeEvent (ScheduledEvent {
            noteStartTime,
            juce::MidiMessage::noteOn (midiChannel, note.noteNumber, static_cast<juce::uint8> (note.velocity)),
            output });

        // Schedule note-off
        writeEvent (ScheduledEvent {
            noteEndTime,
            juce::MidiMessage::noteOff (midiChannel, note.noteNumber),
            output });
    }
}

void TransportEngine::clearScheduledEvents()
{
    // Reset the FIFO by reading all available items
    int numReady = fifo.getNumReady();
    if (numReady > 0)
    {
        int start1, size1, start2, size2;
        fifo.prepareToRead (numReady, start1, size1, start2, size2);
        fifo.finishedRead (size1 + size2);
    }
}

void TransportEngine::writeEvent (const ScheduledEvent& event)
{
    int start1, size1, start2, size2;
    fifo.prepareToWrite (1, start1, size1, start2, size2);

    if (size1 > 0)
    {
        eventBuffer[static_cast<size_t> (start1)] = event;
        fifo.finishedWrite (1);
    }
    else if (size2 > 0)
    {
        eventBuffer[static_cast<size_t> (start2)] = event;
        fifo.finishedWrite (1);
    }
    // If neither has space, event is dropped (queue overflow)
}

// === Per-Track Timing Queries ===

bool TransportEngine::trackNeedsScheduling (size_t trackIndex, double currentPosition) const
{
    if (trackIndex >= numActiveTracks.load())
        return false;

    double nextStart = trackStates[trackIndex].nextLoopStartTime.load();
    return currentPosition >= (nextStart - lookAheadTime);
}

double TransportEngine::getTrackNextLoopStartTime (size_t trackIndex) const
{
    if (trackIndex >= MAX_TRACKS)
        return 0.0;

    return trackStates[trackIndex].nextLoopStartTime.load();
}

void TransportEngine::markTrackScheduled (size_t trackIndex, double loopLengthSeconds)
{
    if (trackIndex >= MAX_TRACKS)
        return;

    auto& state = trackStates[trackIndex];
    double currentStart = state.nextLoopStartTime.load();
    state.nextLoopStartTime.store (currentStart + loopLengthSeconds);
    state.loopLengthSeconds.store (loopLengthSeconds);
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

void TransportEngine::processBlock (double currentPosition, double bufferDuration)
{
    double bufferEndTime = currentPosition + bufferDuration;

    // Read events from the FIFO that fall within this buffer's time window
    int numReady = fifo.getNumReady();
    if (numReady == 0)
        return;

    int start1, size1, start2, size2;
    fifo.prepareToRead (numReady, start1, size1, start2, size2);

    int totalConsumed = 0;

    // Lambda to process a contiguous block of events
    auto processRange = [&] (int start, int size) -> bool
    {
        for (int i = 0; i < size; ++i)
        {
            const auto& event = eventBuffer[static_cast<size_t> (start + i)];

            if (event.timestamp <= bufferEndTime)
            {
                // Send to the event's designated output
                if (event.output != nullptr)
                {
                    event.output->sendMessageNow (event.message);
                }
                ++totalConsumed;
            }
            else
            {
                // Events beyond buffer window - stop processing
                // (we process all events in order, so once we hit one beyond
                // the window, remaining events are also beyond)
                return false;
            }
        }
        return true; // Processed all events in range
    };

    // Process first contiguous block
    bool continueProcessing = processRange (start1, size1);

    // Process second contiguous block (wrap-around) if needed
    if (continueProcessing && size2 > 0)
    {
        processRange (start2, size2);
    }

    fifo.finishedRead (totalConsumed);
}
