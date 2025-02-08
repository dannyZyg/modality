/*
  ==============================================================================

    Sequence.h
    Created: 13 Jan 2025 3:17:18pm
    Author:  Danny Keig

  ==============================================================================
*/

#pragma once
#include "Data/Step.h"
#include "Data/Note.h"
#include "Data/Selectable.h"

class Sequence : public Selectable<Sequence>
{
public:
    using Selectable<Sequence>::Selectable;

    std::vector<std::unique_ptr<Step>> steps;

    int lengthBeats = 4;
    int stepsPerBeat = 4;

    Step& getStep(size_t index);
    size_t getNumNotesInStep(size_t stepIndex);
    size_t nextNoteIndexInStep(size_t stepIndex, size_t noteIndex);
    size_t prevNoteIndexInStep(size_t stepIndex, size_t noteIndex);

    std::pair<int, int> getUsedDegreeRange() const;


    // === Struct to Represent MIDI Notes ===
    struct MidiNote
    {
        double time;    // Note start time (in seconds)
        int noteNumber; // MIDI note number (0-127)
        int velocity;   // Velocity (0-127)
        double duration;// Duration in seconds

        MidiNote(double t, int note, int vel, double dur)
            : time(t), noteNumber(note), velocity(vel), duration(dur) {}
    };

    std::vector<MidiNote> extractMidiNotes();
    std::vector<std::unique_ptr<Note>> notes;

    std::vector<std::reference_wrapper<std::unique_ptr<Note>>> findNotes(double minTime, double maxTime, double minDegree, double maxDegree);
    void removeNotes(double minTime, double maxTime, double minDegree, double maxDegree);

private:
    auto makeNotePredicate(double minTime, double maxTime, double minDegree, double maxDegree);

};
