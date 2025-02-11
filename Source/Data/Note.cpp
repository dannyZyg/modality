/*
  ==============================================================================

    Note.cpp
    Created: 17 Jan 2025 7:00:41am
    Author:  Danny Keig

  ==============================================================================
*/

#include "Note.h"


Note::Note(double deg, double time, double dur) : degree(deg), startTime(time), duration(dur) {}

Note::~Note() { }

double Note::getDegree() const { return degree; }

double Note::getDuration() const { return duration; }

double Note::getOctave() const { return octave; }

double Note::getStartTime() const { return startTime; }

void Note::shiftDegreeUp() { degree++; }

void Note::shiftDegreeDown() { degree--; }

void Note::shiftEarlier(double step) { startTime-= step; }

void Note::shiftLater(double step) { startTime+= step; }

void Note::addModifier(Modifier m)
{
    modifiers.insert(m);
}

std::optional<Modifier> Note::getModifier(ModifierType type) {
    for (const Modifier& mod : modifiers) {
        if (mod.getType() == type) {  // Assuming you can access type or have a getter
            return mod;
        }
    }
    return std::nullopt;
}

bool Note::hasAnyModifier() { return modifiers.size() > 0; }

std::optional<MidiNote> Note::asMidiNote(Timeline t, Scale s, double tempo)
{
    double start = t.convertBarPositionToSeconds(getStartTime(), tempo);
    double dur = t.convertDivisionToSeconds(getDuration(), tempo);
    auto midi = MidiNote(start, 64 + getDegree(), 100, dur);

    auto mod = getModifier(ModifierType::randomTrigger);

    if (mod) {
        auto probability = mod->getModifierValue("percentChanceTriggerValue");
        std::bernoulli_distribution d(any_cast<double>(probability));

        if (d(randomGenerator)) {
            return midi;
        } else {
            return std::nullopt;
        }
    }

    return midi;
}
