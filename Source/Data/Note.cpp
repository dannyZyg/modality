/*
  ==============================================================================

    Note.cpp
    Created: 17 Jan 2025 7:00:41am
    Author:  Danny Keig

  ==============================================================================
*/

#include "Note.h"


Note::Note(double deg, double time, double dur) : degree(deg), startTime(time), duration(dur) {}

Note::~Note() { }

double Note::getDegree() const { return degree; }

double Note::getOctave() const { return octave; }

double Note::getStartTime() const { return startTime; }

void Note::shiftDegreeUp() { degree++; }

void Note::shiftDegreeDown() { degree--; }

void Note::shiftEarlier(double step) { startTime-= step; }

void Note::shiftLater(double step) { startTime+= step; }
