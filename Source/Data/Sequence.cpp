/*
  ==============================================================================

    Sequence.cpp
    Created: 13 Jan 2025 3:17:18pm
    Author:  Danny Keig

  ==============================================================================
*/

#include "Sequence.h"
#include "Data/Note.h"
#include "Data/Scale.h"
#include "Data/Timeline.h"
#include "juce_core/juce_core.h"
#include "juce_data_structures/juce_data_structures.h"

Sequence::Sequence() : Sequence (juce::ValueTree()) {}

Sequence::Sequence (juce::ValueTree existingState) : state (existingState.isValid() ? std::move (existingState) : juce::ValueTree (SequenceIDs::Sequence)),
                                                     timeline (state.getChildWithName (TimelineIDs::Timeline)),
                                                     scale (state.getChildWithName (ScaleIDs::Scale))
{
    // ensure properties exist
    if (! state.hasProperty (SequenceIDs::MidiChannel))
        setMidiChannel (1);

    if (! state.hasProperty (SequenceIDs::LengthBeats))
        setLengthBeats (defaultLengthBeats);

    if (! state.hasProperty (SequenceIDs::MidiOutputId))
        setMidiOutputId ("");

    // ensure child trees exist
    if (! state.getChildWithName (TimelineIDs::Timeline).isValid())
        state.addChild (timeline.getState(), -1, nullptr);

    if (! state.getChildWithName (ScaleIDs::Scale).isValid())
        state.addChild (scale.getState(), -1, nullptr);

    if (! state.getChildWithName (SequenceIDs::Notes).isValid())
        state.addChild (juce::ValueTree (SequenceIDs::Notes), -1, nullptr);

    auto notesState = getNotesState();
    for (int i = 0; i < notesState.getNumChildren(); ++i)
    {
        insertNote (notesState.getChild (i), nullptr);
    }

    state.addListener (this);
}

juce::ValueTree Sequence::ensureChildrenExist (juce::ValueTree s)
{
    return s;
}

Sequence::~Sequence()
{
    state.removeListener (this);
}

juce::ValueTree& Sequence::getState() { return state; }

juce::ValueTree Sequence::getNotesState()
{
    return state.getChildWithName (SequenceIDs::Notes);
}

void Sequence::insertNote (juce::ValueTree v, juce::UndoManager* undoManager)
{
    jassert (v.hasType (NoteIDs::Note));

    // Only add the note if we don't have something in that space yet
    if (isExistingNote (v))
    {
        return;
    }

    undoManager->beginNewTransaction ("insertNote");
    getNotesState().addChild (v, -1, undoManager);
}

bool Sequence::isExistingNote (juce::ValueTree newNote)
{
    auto notesState = getNotesState();

    for (int i = 0; i < notesState.getNumChildren(); ++i)
    {
        auto existingNote = notesState.getChild (i);
        double existingDegree = static_cast<double> (existingNote.getProperty (NoteIDs::Degree));
        double existingStartTime = static_cast<double> (existingNote.getProperty (NoteIDs::StartTime));

        double newDegree = static_cast<double> (newNote.getProperty (NoteIDs::Degree));
        double newStartTime = static_cast<double> (newNote.getProperty (NoteIDs::StartTime));

        if (juce::approximatelyEqual (existingDegree, newDegree) && juce::approximatelyEqual (existingStartTime, newStartTime))
        {
            return true;
        }
    }
    return false;
}

double Sequence::getLengthSeconds (double tempo) const
{
    double secondsPerBeat = 60.0 / tempo;
    return getLengthBeats() * secondsPerBeat;
}

void Sequence::setLengthBeats (double beats, juce::UndoManager* undoManager)
{
    state.setProperty (SequenceIDs::LengthBeats, beats, undoManager);
    // update timeline with new bounds
    timeline.setUpperBound (beats, undoManager);
}

double Sequence::getLengthBeats() const { return static_cast<double> (state.getProperty (SequenceIDs::LengthBeats)); }

int Sequence::getMidiChannel() const { return static_cast<int> (state.getProperty (SequenceIDs::MidiChannel)); }

void Sequence::setMidiChannel (int channel, juce::UndoManager* undoManager)
{
    int midiChannel = juce::jlimit (1, 16, channel);
    state.setProperty (SequenceIDs::MidiChannel, midiChannel, undoManager);
}

void Sequence::setMidiOutputId (const juce::String& outputId, juce::UndoManager* undoManager)
{
    state.setProperty (SequenceIDs::MidiOutputId, outputId, undoManager);
}

juce::String Sequence::getMidiOutputId() const
{
    return state.getProperty (SequenceIDs::MidiOutputId);
}

void Sequence::setEnabled (bool isEnabled)
{
    enabled = isEnabled;
}

bool Sequence::isEnabled() const
{
    return enabled;
}

void Sequence::setMuted (bool isMuted)
{
    muted = isMuted;
}

bool Sequence::isMuted() const
{
    return muted;
}

// Create a reusable predicate to filter notes
auto Sequence::isNoteWithin (double minTime, double maxTime, double minDegree, double maxDegree)
{
    return [=] (const auto& note)
    {
        return Note::isWithinRange (note->getState(), minTime, maxTime, minDegree, maxDegree);
    };
}

std::vector<std::reference_wrapper<std::unique_ptr<Note>>> Sequence::findNotes (
    double minTime,
    double maxTime,
    double minDegree,
    double maxDegree)
{
    std::vector<std::reference_wrapper<std::unique_ptr<Note>>> result;
    auto predicate = isNoteWithin (minTime, maxTime, minDegree, maxDegree);

    for (auto& note : notes)
    {
        if (predicate (note))
        {
            result.push_back (std::ref (note));
        }
    }

    return result;
}

void Sequence::removeNotes (
    double minTime,
    double maxTime,
    double minDegree,
    double maxDegree,
    juce::UndoManager* undoManager)
{
    undoManager->beginNewTransaction ("removeNotes");

    auto notesState = getNotesState();

    for (int i = notesState.getNumChildren() - 1; i >= 0; --i)
    {
        juce::ValueTree noteChild = notesState.getChild (i);
        auto isWithinRange = Note::isWithinRange (noteChild, minTime, maxTime, minDegree, maxDegree);

        if (isWithinRange)
        {
            notesState.removeChild (noteChild, undoManager);
        }
    }
}

void Sequence::valueTreeChildAdded (ValueTree& parentTree,
                                    ValueTree& childWhichHasBeenAdded)
{
    if (parentTree.hasType (SequenceIDs::Notes))
    {
        auto note = std::make_unique<Note> (childWhichHasBeenAdded);
        notes.emplace_back (std::move (note));
    }
}

void Sequence::valueTreeChildRemoved (ValueTree& parentTree,
                                      ValueTree& childWhichHasBeenRemoved,
                                      [[maybe_unused]] int indexFromWhichChildWasRemoved)
{
    if (parentTree.hasType (SequenceIDs::Notes))
    {
        std::erase_if (notes, [&] (const auto& note)
                       { return note->getState() == childWhichHasBeenRemoved; });
    }
}

const Timeline& Sequence::getTimeline() const { return timeline; }

Timeline& Sequence::getTimeline() { return timeline; }

const Scale& Sequence::getScale() const { return scale; }

Scale& Sequence::getScale() { return scale; }

void Sequence::increaseTimelineStepSize()
{
    timeline.increaseStepSize();
}

void Sequence::decreaseTimelineStepSize()
{
    timeline.decreaseStepSize();
}
