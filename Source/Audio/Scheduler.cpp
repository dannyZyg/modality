#include "Scheduler.h"
#include "juce_core/juce_core.h"
#include <algorithm>
#include <cassert>
#include <sys/socket.h>

Scheduler::Scheduler()
{
    for (size_t i = 0; i < MAX_TRACKS; ++i)
    {
        trackStates[i].cachedMidiChannel = static_cast<int> (i + 1);
    }
}

void Scheduler::setNumTracks (size_t numTracks)
{
    numActiveTracks.store (std::min (numTracks, MAX_TRACKS));
}

void Scheduler::scheduleTrack (size_t trackIndex,
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

bool Scheduler::insertEventsSorted (const std::vector<ScheduledEvent>& newEvents)
{
    assert (juce::MessageManager::getInstance()->isThisTheMessageThread());

    if (newEvents.empty())
        return true;

    std::lock_guard<std::mutex> lock (eventMutex);

    BufferSnapshot current = activeSnapshot.load (std::memory_order_acquire);

    // Prepare to insert notes into the buffer which is NOT currently being consumed by the audio thread
    std::array<ScheduledEvent, MAX_EVENTS>& scratch =
        (current.data == bufferA.data()) ? bufferB : bufferA;

    int head = readHead.load();
    int pendingCount = current.count - head;
    int newCount = pendingCount + static_cast<int> (newEvents.size());

    if (newCount > static_cast<int> (MAX_EVENTS))
    {
        juce::Logger::writeToLog ("TransportEngine: event buffer overflow, events dropped");
        return false;
    }

    // Copy pending events from the current buffer into the new buffer
    std::copy (current.data + head, current.data + current.count, scratch.begin());

    // Insert new events after
    std::copy (newEvents.begin(), newEvents.end(), scratch.begin() + pendingCount);

    std::stable_sort (scratch.begin(), scratch.begin() + newCount, [] (const ScheduledEvent& a, const ScheduledEvent& b)
                      { return a.timestamp < b.timestamp; });

    // Point audio thread to new buffer
    activeSnapshot.store ({ scratch.data(), newCount });
    readHead.store (0);

    return true;
}

bool Scheduler::trackNeedsBeatScheduling (size_t trackIndex, double currentBeat) const
{
    if (trackIndex >= numActiveTracks.load())
        return false;

    double lastScheduled = trackStates[trackIndex].lastScheduledBeat.load();
    return currentBeat >= (lastScheduled - Scheduler::SCHEDULE_THRESHOLD_BEATS);
}

void Scheduler::markBeatsScheduled (size_t trackIndex, double endBeat)
{
    if (trackIndex >= MAX_TRACKS)
        return;

    trackStates[trackIndex].lastScheduledBeat.store (endBeat);
}

void Scheduler::setTrackOutput (size_t trackIndex, juce::MidiOutput* output, int midiChannel)
{
    if (trackIndex >= MAX_TRACKS)
        return;

    trackStates[trackIndex].cachedOutput = output;
    trackStates[trackIndex].cachedMidiChannel = midiChannel;
}

void Scheduler::prepareToPlay (double newSampleRate)
{
    sampleRate = newSampleRate;
}

void Scheduler::reset()
{
    assert (juce::MessageManager::getInstance()->isThisTheMessageThread());
    std::lock_guard<std::mutex> lock (eventMutex);

    sendPendingNoteOffs();

    activeSnapshot.store ({ bufferA.data(), 0 }, std::memory_order_release);
    readHead.store (0, std::memory_order_release);

    sendGenericNoteOffMessages();
}

void Scheduler::sendPendingNoteOffs()
{
    BufferSnapshot current = activeSnapshot.load (std::memory_order_acquire);
    int head = readHead.load (std::memory_order_acquire);

    for (int i = head; i < current.count; ++i)
    {
        const auto& event = current.data[static_cast<size_t> (i)];
        if (event.output != nullptr && event.message.isNoteOff())
            event.output->sendMessageNow (event.message);
    }
}

void Scheduler::sendGenericNoteOffMessages()
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
}

void Scheduler::processBlock (double currentPosition, double bufferDuration)
{
    double bufferEndTime = currentPosition + bufferDuration;

    // Walk the sorted event buffer from readHead, firing every event whose
    // timestamp falls within this buffer's time window.  Because the buffer is
    // globally sorted by timestamp, we can stop at the first future event —
    // everything after it is also in the future.
    BufferSnapshot buffer = activeSnapshot.load();
    int head = readHead.load();

    while (head < buffer.count && buffer.data[static_cast<size_t> (head)].timestamp <= bufferEndTime)
    {
        const auto& event = buffer.data[static_cast<size_t> (head)];
        if (event.output != nullptr)
        {
            event.output->sendMessageNow (event.message);
        }
        ++head;
    }

    readHead.store (head, std::memory_order_release);
}
