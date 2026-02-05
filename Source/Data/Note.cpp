/*
  ==============================================================================

    Note.cpp
    Created: 17 Jan 2025 7:00:41am
    Author:  Danny Keig

  ==============================================================================
*/

#include "Note.h"
#include "Data/ModifierApplicator.h"

Note::Note (double deg, double time, double dur) : degree (deg), startTime (time), duration (dur) {}

Note::~Note() {}

double Note::getDegree() const { return degree; }

double Note::getDuration() const { return duration; }

double Note::getOctave() const { return octave; }

double Note::getStartTime() const { return startTime; }

void Note::shiftDegreeUp() { degree++; }

void Note::shiftDegreeDown() { degree--; }

void Note::shiftEarlier (double step) { startTime -= step; }

void Note::shiftLater (double step) { startTime += step; }

void Note::addModifier (Modifier m)
{
    modifiers.insert (m);
}

bool Note::removeModifier (Modifier m)
{
    return modifiers.erase (m) > 0;
}

std::optional<Modifier> Note::getModifier (ModifierType type)
{
    for (const Modifier& mod : modifiers)
    {
        if (mod.getType() == type)
        { // Assuming you can access type or have a getter
            return mod;
        }
    }
    return std::nullopt;
}

bool Note::hasAnyModifier() { return modifiers.size() > 0; }

int Note::getVelocity() const { return velocity; }

void Note::setVelocity (int v)
{
    velocity = v;
}

std::optional<MidiNote> Note::asMidiNote (Timeline t, [[maybe_unused]] Scale s, double tempo)
{
    double start = t.convertBarPositionToSeconds (getStartTime(), tempo);
    double dur = t.convertDivisionToSeconds (getDuration(), tempo);
    auto midi = MidiNote (start, static_cast<int> (64 + getDegree()), velocity, dur);

    return ModifierApplicator::getInstance().applyModifiers (modifiers, std::move (midi));
}
