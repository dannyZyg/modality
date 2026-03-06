/*
  ==============================================================================

    Note.cpp
    Created: 17 Jan 2025 7:00:41am
    Author:  Danny Keig

  ==============================================================================
*/

#include "Note.h"
#include "Data/ModifierApplicator.h"
#include "juce_data_structures/juce_data_structures.h"
#include <vector>

Note::Note (double deg, double time, double dur) : state (NoteIDs::Note)
{
    state.setProperty (NoteIDs::Degree, deg, nullptr);
    state.setProperty (NoteIDs::StartTime, time, nullptr);
    state.setProperty (NoteIDs::Duration, dur, nullptr);
    state.setProperty (NoteIDs::Velocity, 100, nullptr);
    state.setProperty (NoteIDs::Octave, 0.0, nullptr);
}

Note::Note (juce::ValueTree existingState)
    : state (std::move (existingState))
{
    jassert (state.hasType (NoteIDs::Note));
}

Note::~Note() {}

juce::ValueTree& Note::getState() { return state; }

double Note::getDegree() const { return static_cast<double> (state.getProperty (NoteIDs::Degree)); }

double Note::getDuration() const { return static_cast<double> (state.getProperty (NoteIDs::Duration)); }

double Note::getOctave() const { return static_cast<double> (state.getProperty (NoteIDs::Octave)); }

double Note::getStartTime() const { return static_cast<double> (state.getProperty (NoteIDs::StartTime)); }

void Note::shiftDegreeUp (juce::UndoManager* undoManager)
{
    double current = getDegree();
    state.setProperty (NoteIDs::Degree, current + 1.0, undoManager);
}

void Note::shiftDegreeDown (juce::UndoManager* undoManager)
{
    double current = getDegree();
    state.setProperty (NoteIDs::Degree, current - 1.0, undoManager);
}

void Note::shiftEarlier (double step, juce::UndoManager* undoManager)
{
    double current = getStartTime();
    state.setProperty (NoteIDs::StartTime, current - step, undoManager);
}

void Note::shiftLater (double step, juce::UndoManager* undoManager)
{
    double current = getStartTime();
    state.setProperty (NoteIDs::StartTime, current + step, undoManager);
}

void Note::addModifier (Modifier m, UndoManager* undoManager)
{
    state.getOrCreateChildWithName (m.getType(), undoManager);
}

bool Note::removeModifier (ModifierType type, UndoManager* undoManager)
{
    for (int i = 0; i < state.getNumChildren(); i++)
    {
        if (state.getChild (i).getType() == type)
        {
            state.removeChild (i, undoManager);
            return true;
        }
    }
    return false;
}

std::optional<Modifier> Note::getModifier (ModifierType type)
{
    auto child = state.getChildWithName (type);
    if (child.isValid())
    {
        return Modifier (child);
    }
    return std::nullopt;
}

bool Note::hasAnyModifier() { return state.getNumChildren() > 0; }

int Note::getVelocity() const { return static_cast<int> (state.getProperty (NoteIDs::Velocity)); }

void Note::setVelocity (int v, juce::UndoManager* undoManager)
{
    state.setProperty (NoteIDs::Velocity, v, undoManager);
}

std::optional<MidiNote> Note::asMidiNote (Timeline t, [[maybe_unused]] Scale s, double tempo)
{
    double start = t.convertBarPositionToSeconds (getStartTime(), tempo);
    double dur = t.convertDivisionToSeconds (getDuration(), tempo);
    auto midi = MidiNote (start, static_cast<int> (64 + getDegree()), getVelocity(), dur);

    std::vector<Modifier> modifiers;
    for (int i = 0; i < state.getNumChildren(); i++)
    {
        auto child = state.getChild (i);
        auto modifier = Modifier (child);
        modifiers.push_back (modifier);
    }

    return ModifierApplicator::getInstance().applyModifiers (modifiers, std::move (midi));
}
