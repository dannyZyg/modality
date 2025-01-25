/*
  ==============================================================================

    Sequence.h
    Created: 13 Jan 2025 3:17:18pm
    Author:  Danny Keig

  ==============================================================================
*/

#include "Data/Step.h"
#pragma once

class Sequence
{
public:
    Sequence();
    std::vector<std::unique_ptr<Step>> steps;

    void setIsSelectedCallback(std::function<bool(const Step&)>, std::function<bool(const Note&)>);

    int lengthBeats = 4;
    int stepsPerBeat = 4;

    Step& getStep(size_t index);
    size_t getNumNotesInStep(size_t stepIndex);
    size_t nextNoteIndexInStep(size_t stepIndex, size_t noteIndex);
    size_t prevNoteIndexInStep(size_t stepIndex, size_t noteIndex);

    std::pair<int, int> getUsedDegreeRange() const;

private:

};
