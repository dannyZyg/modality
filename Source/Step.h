/*
  ==============================================================================

    Step.h
    Created: 10 Jan 2025 11:01:43am
    Author:  Danny Keig

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
//#include <juce_graphics/juce_graphics.h>


//==============================================================================
/*
*/
class Step  : public juce::Component
{
public:
    Step();
    ~Step();

    void stepUp();
    void stepDown();
    bool isVisuallySelected = false;
    
    bool isSelected() const;
    void setIsSelectedCallback(std::function<bool(const Step&)> callback);

private:
    int stepValue = 20;
    int shapeHeight = 40;
    juce::Point<int> pos{0, 0};
    
    std::function<bool(const Step&)> isSelectedCallback = nullptr;
};
