/*
  ==============================================================================

    StepView.h
    Created: 13 Jan 2025 3:18:41pm
    Author:  Danny Keig

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Step.h"

//==============================================================================
/*
*/
class StepView  : public juce::Component
{
public:
    StepView(const Step& s);
    ~StepView() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    void setPos(int range);

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StepView)
    
    int stepValue = 20;
    int shapeHeight = 40;
    juce::Point<int> pos{0, 0};
    
    const Step& step;
    
    
};
