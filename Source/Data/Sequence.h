/*
  ==============================================================================

    Sequence.h
    Created: 13 Jan 2025 3:17:18pm
    Author:  Danny Keig

  ==============================================================================
*/

#pragma once
#include "Data/Note.h"

namespace SequenceIDs
{
#define DECLARE_ID(name) inline const juce::Identifier name { #name };
DECLARE_ID (lengthBeats)
DECLARE_ID (midiChannel)
DECLARE_ID (midiOutputId)
#undef DECLARE_ID
} // namespace SequenceIDs

static constexpr double defaultLengthBeats = 4.0f;

class Sequence
{
public:
    Sequence();
    ~Sequence();

    double getLengthSeconds (double tempo) const;
    double getLengthBeats() const;
    void setLengthBeats (double beats, juce::UndoManager* undoManager = nullptr);

    // MIDI output device identifier. Empty string means use the default output device.
    juce::String midiOutputId = "";

    bool enabled = true;
    bool muted = false;

    void setMidiChannel (int channel, juce::UndoManager* undoManager = nullptr);
    int getMidiChannel() const;
    void setMidiOutputId (const juce::String& outputId);
    const juce::String& getMidiOutputId() const;

    void setEnabled (bool isEnabled);
    bool isEnabled() const;

    void setMuted (bool isMuted);
    bool isMuted() const;

    std::vector<std::unique_ptr<Note>> notes;

    std::vector<std::reference_wrapper<std::unique_ptr<Note>>> findNotes (double minTime, double maxTime, double minDegree, double maxDegree);
    void removeNotes (double minTime, double maxTime, double minDegree, double maxDegree);

    const Timeline& getTimeline() const;
    const Scale& getScale() const;
    Timeline& getTimeline();
    Scale& getScale();

    void increaseTimelineStepSize();
    void decreaseTimelineStepSize();

    TimePoint getNextTimelineStep (const TimePoint& tp);
    TimePoint getNextTimelineStep (const TimePoint& tp, double division);

    TimePoint getPrevTimelineStep (const TimePoint& tp);
    TimePoint getPrevTimelineStep (const TimePoint& tp, double division);

private:
    auto isNoteWithin (double minTime, double maxTime, double minDegree, double maxDegree);

    juce::ValueTree state;

    Timeline timeline;
    Scale scale { "Natural Minor" };
};
