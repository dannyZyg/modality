/*
  ==============================================================================

    Step.cpp
    Created: 10 Jan 2025 11:01:43am
    Author:  Danny Keig

  ==============================================================================
*/

#include <JuceHeader.h>
#include "Step.h"

//==============================================================================
Step::Step()
{
}

Step::~Step()
{
}

void Step::stepUp() { --stepValue; }  // Move up the y axis

void Step::stepDown() { ++stepValue; }  // Move down the y axis

bool Step::isSelected() const {
    return isSelectedCallback ? isSelectedCallback(*this) : false;
}

void Step::setIsSelectedCallback(std::function<bool (const Step &)> callback)
{
    isSelectedCallback = callback;
}
