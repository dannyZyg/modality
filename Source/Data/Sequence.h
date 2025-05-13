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

    float lengthBeats = 4.0;

    std::vector<std::unique_ptr<Note>> notes;

    std::vector<std::reference_wrapper<std::unique_ptr<Note>>> findNotes (double minTime, double maxTime, double minDegree, double maxDegree);
    void removeNotes (double minTime, double maxTime, double minDegree, double maxDegree);

    const Timeline& getTimeline() const;
    const Scale& getScale() const;
    Timeline& getTimeline();
    Scale& getScale();

    void increaseTimelineStepSize();
    void decreaseTimelineStepSize();

    TimePoint getNextTimelineStep (const TimePoint& tp);
    TimePoint getNextTimelineStep (const TimePoint& tp, double division);

    TimePoint getPrevTimelineStep (const TimePoint& tp);
    TimePoint getPrevTimelineStep (const TimePoint& tp, double division);

private:
    auto makeNotePredicate (double minTime, double maxTime, double minDegree, double maxDegree);

    Timeline timeline { 0.0, lengthBeats };
    Scale scale { "Natural Minor" };
};
