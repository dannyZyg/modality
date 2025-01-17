/*
  ==============================================================================

    Sequence.cpp
    Created: 13 Jan 2025 3:17:18pm
    Author:  Danny Keig

  ==============================================================================
*/

#include "Sequence.h"

Sequence::Sequence()
{
    for (int i = 0; i < lengthBeats * stepsPerBeat; ++i) {
        auto s = std::make_unique<Step>();
        steps.emplace_back(std::move(s));
    }
};

void Sequence::setIsSelectedCallback(std::function<bool(const Step&)> stepCallback, std::function<bool(const Note&)> noteCallback) {
    for (auto& step : steps) {
        step->setIsSelectedCallback(stepCallback);
        
        for (auto& note: step->notes) {
            note->setIsSelectedCallback(noteCallback);
        }
    }
}

Step& Sequence::getStep(size_t index)
{
    if (index >= steps.size()) {
        std::string m = "Step index out of bounds: " + std::to_string(index);
        juce::Logger::writeToLog(m);
        throw std::out_of_range(m);
    }
    return *steps[index];
}

size_t Sequence::getNumNotesInStep(size_t stepIndex)
{
    Step& step = getStep(stepIndex);
    return step.notes.size();
}

size_t Sequence::nextNoteIndexInStep(size_t stepIndex , size_t noteIndex)
{
    // find the next note index for the step, wrapping to the first index if we are at the end
    size_t numNotes = getNumNotesInStep(stepIndex);

    if (noteIndex < numNotes - 1) {
        return noteIndex + 1;
    } else {
        return 0;
    }
}

size_t Sequence::prevNoteIndexInStep(size_t stepIndex , size_t noteIndex)
{
    // find the prev note index for the step, wrapping to the last index if we are at 0
    size_t numNotes = getNumNotesInStep(stepIndex);

    if (noteIndex <= 0) {
        return noteIndex - 1;
    } else {
        return numNotes - 1;
    }
}
