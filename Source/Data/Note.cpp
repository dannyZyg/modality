/*
  ==============================================================================

    Note.cpp
    Created: 17 Jan 2025 7:00:41am
    Author:  Danny Keig

  ==============================================================================
*/

#include "Note.h"


Note::~Note()
{

}

int Note::getDegree() const { return degree; }

int Note::getOctave() const { return octave; }

void Note::shiftDegreeUp()
{
    degree++;
}

void Note::shiftDegreeDown()
{
    juce::Logger::writeToLog("Deg was: " + juce::String(degree));

    degree--;
    juce::Logger::writeToLog("Deg  is: " + juce::String(degree));
}
