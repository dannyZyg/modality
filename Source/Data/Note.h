/*
  ==============================================================================

    Note.h
    Created: 17 Jan 2025 7:00:41am
    Author:  Danny Keig

  ==============================================================================
*/

#include "Data/Modifier.h"
#include "Data/Scale.h"
#include "Data/Timeline.h"
#include <JuceHeader.h>
#include <random>
#pragma once

// === Struct to Represent MIDI Notes ===
struct MidiNote
{
    double startTime; // Note start time (in seconds)
    int noteNumber; // MIDI note number (0-127)
    int velocity; // Velocity (0-127)
    double duration; // Duration in seconds

    MidiNote (double t, int note, int vel, double dur)
        : startTime (t), noteNumber (note), velocity (vel), duration (dur) {}
};

class Note
{
public:
    Note (double deg, double time, double dur);
    ~Note();

    double getDegree() const;
    double getDuration() const;
    double getOctave() const;
    double getStartTime() const;
    int getVelocity() const;
    void setVelocity (int v);
    void shiftDegreeUp();
    void shiftDegreeDown();
    void shiftEarlier (double step);
    void shiftLater (double step);

    void addModifier (Modifier m);
    bool removeModifier (Modifier m);
    std::optional<Modifier> getModifier (ModifierType type);

    bool hasAnyModifier();
    std::optional<MidiNote> asMidiNote (Timeline t, Scale s, double tempo);

private:
    double degree = 0.0;
    double startTime = 0.0;
    double octave = 0;
    int velocity = 100;
    double duration = Division::sixteenth;

    std::unordered_set<Modifier> modifiers;
    std::mt19937 randomGenerator;
};
