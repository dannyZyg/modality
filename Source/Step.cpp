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
    //isSelectedCallback = nullptr;
}

void Step::stepUp() { pos.y = pos.y - stepValue; }

void Step::stepDown() { pos.y = pos.y + stepValue; }

bool Step::isSelected() const {
    return isSelectedCallback ? isSelectedCallback(*this) : false;
}

void Step::setIsSelectedCallback(std::function<bool (const Step &)> callback)
{
    isSelectedCallback = callback;
}
