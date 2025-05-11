/*
  ==============================================================================

    Sequence.h
    Created: 13 Jan 2025 3:17:18pm
    Author:  Danny Keig

  ==============================================================================
*/

#pragma once
#include "Data/Note.h"

class Sequence
{
public:
    Sequence();
    ~Sequence();

    int lengthBeats = 4;

    std::vector<std::unique_ptr<Note>> notes;

    std::vector<std::reference_wrapper<std::unique_ptr<Note>>> findNotes (double minTime, double maxTime, double minDegree, double maxDegree);
    void removeNotes (double minTime, double maxTime, double minDegree, double maxDegree);

private:
    auto makeNotePredicate (double minTime, double maxTime, double minDegree, double maxDegree);
};
