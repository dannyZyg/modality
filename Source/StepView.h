/*
  ==============================================================================

    StepView.h
    Created: 13 Jan 2025 3:18:41pm
    Author:  Danny Keig

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class StepView  : public juce::Component
{
public:
    StepView();
    ~StepView() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StepView)
};
