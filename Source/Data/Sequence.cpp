/*
  ==============================================================================

    Sequence.cpp
    Created: 13 Jan 2025 3:17:18pm
    Author:  Danny Keig

  ==============================================================================
*/

#include "Sequence.h"
#include "juce_core/system/juce_PlatformDefs.h"

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

    if (noteIndex >= 1) {
        return noteIndex - 1;
    } else {
        return numNotes - 1;
    }
}

std::pair<int, int> Sequence::getUsedDegreeRange() const
{
    // Find the highest and lowest degrees of all the notes in all of the steps;
    int min = 0;
    int max = 0;
    for (auto& step : steps) {
        for (auto& note : step->notes) {
            if (note->getDegree() > max)
                max = note->getDegree();

            if (note->getDegree() < min)
                min = note->getDegree();
        }
    }

    return {min, max};
}

std::vector<Sequence::MidiNote> Sequence::extractMidiNotes()
{

    std::vector<MidiNote> midiClip;

    float tempo = 120.0;

    double secondsPerBeat = 60.0 / tempo;

    for (auto& n : notes) {
        double beats = n->getStartTime() * 4;
        double time = beats * secondsPerBeat;
        DBG("time: " << time) ;

        midiClip.emplace_back(time, 64 + n->getDegree(), 100, 1 * 0.9);
    }
    return midiClip;
}

// Create a reusable predicate
auto Sequence::makeNotePredicate(double minTime, double maxTime, double minDegree, double maxDegree) {
    return [=](const auto& note) {
        return note->getStartTime() >= minTime && note->getStartTime() < maxTime &&
               note->getDegree() >= minDegree && note->getDegree() <= maxDegree;
    };
}

std::vector<std::reference_wrapper<std::unique_ptr<Note>>> Sequence::findNotes(
    double minTime, double maxTime,
    double minDegree, double maxDegree) {

    std::vector<std::reference_wrapper<std::unique_ptr<Note>>> result;
    auto predicate = makeNotePredicate(minTime, maxTime, minDegree, maxDegree);

    for (auto& note : notes) {
        if (predicate(note)) {
            result.push_back(std::ref(note));
        }
    }

    return result;
}

void Sequence::removeNotes(
    double minTime, double maxTime,
    double minDegree, double maxDegree) {

    auto predicate = makeNotePredicate(minTime, maxTime, minDegree, maxDegree);
    notes.erase(
        std::remove_if(notes.begin(), notes.end(), predicate),
        notes.end()
    );
}
