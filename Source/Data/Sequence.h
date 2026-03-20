/*
  ==============================================================================

    Sequence.h
    Created: 13 Jan 2025 3:17:18pm
    Author:  Danny Keig

  ==============================================================================
*/

#pragma once
#include "Data/Note.h"
#include "juce_data_structures/juce_data_structures.h"

namespace SequenceIDs
{
#define DECLARE_ID(name) inline const juce::Identifier name { #name };
DECLARE_ID (Sequence)
DECLARE_ID (Notes)
DECLARE_ID (LengthBeats)
DECLARE_ID (MidiChannel)
DECLARE_ID (MidiOutputId)
#undef DECLARE_ID
} // namespace SequenceIDs

static constexpr double defaultLengthBeats = 4.0f;

class Sequence : juce::ValueTree::Listener
{
public:
    Sequence();
    explicit Sequence (juce::ValueTree existingState);
    ~Sequence();

    juce::ValueTree& getState();
    void loadNotesFromState();

    double getLengthSeconds (double tempo) const;
    double getLengthBeats() const;
    void setLengthBeats (double beats, juce::UndoManager* undoManager = nullptr);

    bool enabled = true;
    bool muted = false;

    void setMidiChannel (int channel, juce::UndoManager* undoManager = nullptr);
    int getMidiChannel() const;
    void setMidiOutputId (const juce::String& outputId, juce::UndoManager* undoManager = nullptr);

    juce::Value getMidiOutputIdAsValue();
    juce::Value getMidiOutputChannelAsValue();

    juce::String getMidiOutputId() const;

    void setEnabled (bool isEnabled);
    bool isEnabled() const;

    void setMuted (bool isMuted);
    bool isMuted() const;

    std::vector<std::unique_ptr<Note>> notes;

    void valueTreeChildAdded (ValueTree& parentTree,
                              ValueTree& childWhichHasBeenAdded);

    void valueTreeChildRemoved (ValueTree& parentTree,
                                ValueTree& childWhichHasBeenRemoved,
                                int indexFromWhichChildWasRemoved);

    std::vector<std::reference_wrapper<std::unique_ptr<Note>>> findNotes (double minTime, double maxTime, double minDegree, double maxDegree);
    void removeNotes (double minTime, double maxTime, double minDegree, double maxDegree, juce::UndoManager* undoManager);
    void insertNote (juce::ValueTree v, juce::UndoManager* undoManager = nullptr);
    bool isExistingNote (juce::ValueTree noteState);

    const Timeline& getTimeline() const;
    const Scale& getScale() const;
    Timeline& getTimeline();
    Scale& getScale();

    void increaseTimelineStepSize();
    void decreaseTimelineStepSize();

private:
    juce::ValueTree ensureChildrenExist (juce::ValueTree s);
    auto isNoteWithin (double minTime, double maxTime, double minDegree, double maxDegree);

    juce::ValueTree state;
    juce::ValueTree getNotesState();

    Timeline timeline;
    Scale scale { "Natural Minor" };
};
