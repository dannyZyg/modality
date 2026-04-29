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
#include <algorithm>
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

double Note::getDegree() const { return state.getProperty (NoteIDs::Degree); }

double Note::getDuration() const { return state.getProperty (NoteIDs::Duration); }

double Note::getOctave() const { return state.getProperty (NoteIDs::Octave); }

double Note::getStartTime() const { return state.getProperty (NoteIDs::StartTime); }

int Note::getVelocity() const { return state.getProperty (NoteIDs::Velocity); }

void Note::setStartTime (double value, juce::UndoManager* undoManager)
{
    state.setProperty (NoteIDs::StartTime, value, undoManager);
}

void Note::setDegree (double value, juce::UndoManager* undoManager)
{
    state.setProperty (NoteIDs::Degree, value, undoManager);
}

void Note::setVelocity (int v, juce::UndoManager* undoManager)
{
    state.setProperty (NoteIDs::Velocity, v, undoManager);
}

void Note::setDuration (double value, juce::UndoManager* undoManager)
{
    state.setProperty (NoteIDs::Duration, value, undoManager);
}

void Note::setLastTriggeredMidiNote (const MidiNote& m)
{
    lastTriggeredMidiNote = m;
}

void Note::clearLastTriggeredMidiNote()
{
    lastTriggeredMidiNote.reset();
}

void Note::addModifier (Modifier m, UndoManager* undoManager)
{
    if (! state.getChildWithName (m.getType()).isValid())
        state.appendChild (m.getState(), undoManager);
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

bool Note::hasAnyModifier()
{
    for (const auto& type : ModifierIDs::AllTypes)
    {
        if (state.getChildWithName (type).isValid())
            return true;
    }

    return false;
}

std::optional<MidiNote> Note::asMidiNote (Timeline t, [[maybe_unused]] Scale s, double tempo, int rootNote)
{
    double start = t.convertBarPositionToSeconds (getStartTime(), tempo);
    double dur = t.convertDivisionToSeconds (getDuration(), tempo);
    auto midi = MidiNote (start, static_cast<int> (rootNote + getDegree()), getVelocity(), dur);

    // Create thread-safe parameter snapshots to avoid race conditions during modifier application
    std::vector<ModifierParameterSnapshot> modifierSnapshots;
    for (int i = 0; i < state.getNumChildren(); i++)
    {
        auto child = state.getChild (i);
        auto modifier = Modifier (child);
        modifierSnapshots.push_back (modifier.createParameterSnapshot());
    }

    // Sort snapshots by their position in AllTypes so execution order is deterministic
    // (e.g. RandomPitchVariation always runs before RandomOctaveShift)
    std::sort (modifierSnapshots.begin(), modifierSnapshots.end(), [] (const ModifierParameterSnapshot& a, const ModifierParameterSnapshot& b)
               {
        auto indexOf = [] (const juce::Identifier& id)
        {
            const auto& types = ModifierIDs::AllTypes;
            auto it = std::find (types.begin(), types.end(), id);
            return it != types.end() ? std::distance (types.begin(), it) : static_cast<ptrdiff_t> (types.size());
        };
        return indexOf (a.type) < indexOf (b.type); });

    MidiNote finalMidi = ModifierApplicator::getInstance().applyModifiersThreadSafe (modifierSnapshots, std::move (midi), s);
    return finalMidi;
}
