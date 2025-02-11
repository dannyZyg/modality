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

}

Sequence::~Sequence()
{

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
