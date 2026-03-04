/*
  ==============================================================================

    Sequence.cpp
    Created: 13 Jan 2025 3:17:18pm
    Author:  Danny Keig

  ==============================================================================
*/

#include "Sequence.h"
#include "Data/Note.h"
#include "Data/Timeline.h"
#include "juce_core/juce_core.h"

Sequence::Sequence() : state (SequenceIDs::Sequence), notesState (NotesStateIDs::NotesState), timeline (0, defaultLengthBeats)
{
    state.setProperty (SequenceIDs::midiChannel, 1, nullptr);
    state.setProperty (SequenceIDs::midiOutputId, "", nullptr);
    state.setProperty (SequenceIDs::lengthBeats, defaultLengthBeats, nullptr);
    state.addChild (timeline.getState(), -1, nullptr);
    state.addChild (scale.getState(), -1, nullptr);
    state.addChild (notesState, -1, nullptr);
    state.addListener (this);
}

Sequence::~Sequence()
{
    state.removeListener (this);
}

void Sequence::insertNote (juce::ValueTree& v, juce::UndoManager* undoManager)
{
    jassert (v.hasType (NoteIDs::Note));

    // Only add the note if we don't have something in that space yet
    if (isExistingNote (v))
    {
        return;
    }

    notesState.addChild (v, -1, undoManager);
}

bool Sequence::isExistingNote (juce::ValueTree newNote)
{
    for (int i = 0; i < notesState.getNumChildren(); ++i)
    {
        auto existingNote = notesState.getChild (i);
        double existingDegree = static_cast<double> (existingNote.getProperty (NoteIDs::degree));
        double existingStartTime = static_cast<double> (existingNote.getProperty (NoteIDs::startTime));

        double newDegree = static_cast<double> (newNote.getProperty (NoteIDs::degree));
        double newStartTime = static_cast<double> (newNote.getProperty (NoteIDs::startTime));

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
    state.setProperty (SequenceIDs::lengthBeats, beats, undoManager);
    // update timeline with new bounds
    timeline.setUpperBound (beats, undoManager);
}

double Sequence::getLengthBeats() const { return static_cast<double> (state.getProperty (SequenceIDs::lengthBeats)); }

int Sequence::getMidiChannel() const { return static_cast<int> (state.getProperty (SequenceIDs::midiChannel)); }

void Sequence::setMidiChannel (int channel, juce::UndoManager* undoManager)
{
    int midiChannel = juce::jlimit (1, 16, channel);
    state.setProperty (SequenceIDs::midiChannel, midiChannel, undoManager);
}

void Sequence::setMidiOutputId (const juce::String& outputId)
{
    midiOutputId = outputId;
}

const juce::String& Sequence::getMidiOutputId() const
{
    return midiOutputId;
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
    double maxDegree)
{
    for (int i = notesState.getNumChildren() - 1; i >= 0; --i)
    {
        juce::ValueTree noteChild = notesState.getChild (i);
        auto isWithinRange = Note::isWithinRange (noteChild, minTime, maxTime, minDegree, maxDegree);

        if (isWithinRange)
        {
            notesState.removeChild (noteChild, nullptr);
        }
    }
}

void Sequence::valueTreeChildAdded (ValueTree& parentTree,
                                    ValueTree& childWhichHasBeenAdded)
{
    if (parentTree.hasType (NotesStateIDs::NotesState))
    {
        auto note = std::make_unique<Note> (childWhichHasBeenAdded);
        notes.emplace_back (std::move (note));
    }
}

void Sequence::valueTreeChildRemoved (ValueTree& parentTree,
                                      ValueTree& childWhichHasBeenRemoved,
                                      [[maybe_unused]] int indexFromWhichChildWasRemoved)
{
    if (parentTree.hasType (NotesStateIDs::NotesState))
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

TimePoint Sequence::getNextTimelineStep (const TimePoint& tp)
{
    return timeline.getNextStep (tp);
}

TimePoint Sequence::getNextTimelineStep (const TimePoint& tp, double division)
{
    return timeline.getNextStep (tp, division);
}

TimePoint Sequence::getPrevTimelineStep (const TimePoint& tp)
{
    return timeline.getPrevStep (tp);
}

TimePoint Sequence::getPrevTimelineStep (const TimePoint& tp, double division)
{
    return timeline.getPrevStep (tp, division);
}
