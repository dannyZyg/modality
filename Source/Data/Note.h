/*
  ==============================================================================

    Note.h
    Created: 17 Jan 2025 7:00:41am
    Author:  Danny Keig

  ==============================================================================
*/


#include <JuceHeader.h>
#pragma once

class Note
{
public:
    Note(double deg, double time, double dur);
    ~Note();

    double getDegree() const;
    double getOctave() const;
    double getStartTime() const;
    void shiftDegreeUp();
    void shiftDegreeDown();
    void shiftEarlier(double step);
    void shiftLater(double step);

private:
    double degree = 0.0;
    double startTime = 0.0;
    double octave = 0;
    double duration;

};
