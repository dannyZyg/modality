/*
  ==============================================================================

    Sequence.h
    Created: 13 Jan 2025 3:17:18pm
    Author:  Danny Keig

  ==============================================================================
*/

#include "Step.h"
#pragma once

class Sequence
{
public:
    Sequence();
    std::vector<std::unique_ptr<Step>> steps;
    
    void setIsSelectedCallback(std::function<bool(const Step&)>);

private:
    int lengthBeats = 4;
    int stepsPerBeat = 4;
    
};
