/*
  ==============================================================================

    Note.h
    Created: 17 Jan 2025 7:00:41am
    Author:  Danny Keig

  ==============================================================================
*/

#pragma once

#include "Data/Modifier.h"
#include "Data/Scale.h"
#include "Data/Timeline.h"
#include "juce_data_structures/juce_data_structures.h"
#include <JuceHeader.h>
#include <random>

namespace NoteIDs
{
#define DECLARE_ID(name) inline const juce::Identifier name { #name };
DECLARE_ID (Note)
DECLARE_ID (Degree)
DECLARE_ID (StartTime)
DECLARE_ID (Duration)
DECLARE_ID (Octave)
DECLARE_ID (Velocity)
#undef DECLARE_ID
} // namespace NoteIDs

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
    Note (double deg = 0.0, double time = 0.0, double dur = Division::sixteenth);
    explicit Note (juce::ValueTree existingState);

    ~Note();

    Note (const Note&) = default;
    Note& operator= (const Note&) = default;
    Note (Note&&) = default;
    Note& operator= (Note&&) = default;

    static bool isWithinRange (juce::ValueTree state, double minTime, double maxTime, double minDegree, double maxDegree)
    {
        double startTime = static_cast<double> (state.getProperty (NoteIDs::StartTime));
        double degree = static_cast<double> (state.getProperty (NoteIDs::Degree));
        return startTime >= minTime && startTime < maxTime && degree >= minDegree && degree <= maxDegree;
    }

    juce::ValueTree& getState();
    double getDegree() const;
    double getDuration() const;
    double getOctave() const;
    double getStartTime() const;
    int getVelocity() const;
    void setVelocity (int v, juce::UndoManager* undoManager = nullptr);
    void shiftDegreeUp (juce::UndoManager* undoManager = nullptr);
    void shiftDegreeDown (juce::UndoManager* undoManager = nullptr);
    void shiftEarlier (double step, juce::UndoManager* undoManager = nullptr);
    void shiftLater (double step, juce::UndoManager* undoManager = nullptr);

    void addModifier (Modifier m, UndoManager* undoManager = nullptr);
    bool removeModifier (ModifierType type, UndoManager* undoManager = nullptr);
    std::optional<Modifier> getModifier (ModifierType type);

    bool hasAnyModifier();
    std::optional<MidiNote> asMidiNote (Timeline t, Scale s, double tempo);

private:
    juce::ValueTree state;
    std::mt19937 randomGenerator;
};
