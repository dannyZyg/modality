#pragma once

#include "Audio/ScheduledEvent.h"
#include "Data/Note.h"
#include "juce_audio_devices/juce_audio_devices.h"
#include <array>
#include <atomic>

class Scheduler
{
public:
    struct BufferSnapshot
    {
        ScheduledEvent* data = nullptr;
        int count = 0;
    };

    static constexpr size_t MAX_TRACKS = 16;
    static constexpr size_t MAX_EVENTS = 4096;
    static constexpr double LOOKAHEAD_BEATS = 1.5; // Schedule this many beats ahead
    static constexpr double SCHEDULE_THRESHOLD_BEATS = 0.0; // Start scheduling when within this many beats

    Scheduler();
    ~Scheduler() = default;

    void setNumTracks (size_t numTracks);

    void scheduleTrack (size_t trackIndex,
                        const std::vector<MidiNote>& notes,
                        double loopStartTime,
                        juce::MidiOutput* output,
                        int midiChannel);

    bool trackNeedsBeatScheduling (size_t trackIndex, double currentBeat) const;
    void markBeatsScheduled (size_t trackIndex, double endBeat);
    void setTrackOutput (size_t trackIndex, juce::MidiOutput* output, int midiChannel);
    void prepareToPlay (double sampleRate);
    void reset();
    void processBlock (double currentPosition, double bufferDuration);

    void sendPendingNoteOffs();
    void sendGenericNoteOffMessages();

private:
    std::mutex eventMutex;
    std::atomic<bool> wasPlaying { false };

    std::array<ScheduledEvent, MAX_EVENTS> bufferA, bufferB;
    std::atomic<BufferSnapshot> activeSnapshot;
    std::atomic<int> readHead { 0 }; // audio thread's current read position

    std::array<PerTrackState, MAX_TRACKS> trackStates;
    std::atomic<size_t> numActiveTracks { 4 };

    double sampleRate { 44100.0 };

    bool insertEventsSorted (const std::vector<ScheduledEvent>& newEvents);

    // juce::AbstractFifo debugLog;
};
