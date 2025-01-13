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
    ~Step() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void select();
    void unselect();
    void stepUp();
    void stepDown();
    bool isVisuallySelected = false;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Step)
    
    int stepValue = 20;
    int shapeHeight = 40;
    bool isSelected = false;
    juce::Point<int> pos{0, 0};

};
