/*
  ==============================================================================

    Sequence.h
    Created: 13 Jan 2025 3:17:18pm
    Author:  Danny Keig

  ==============================================================================
*/

#include "Data/Step.h"
#include "Data/Selectable.h"
#pragma once

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

private:

};
