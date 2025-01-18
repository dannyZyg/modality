/*
  ==============================================================================

    StepComponent.h
    Created: 13 Jan 2025 3:18:41pm
    Author:  Danny Keig

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "NoteComponent.h"
#include "Step.h"

//==============================================================================
/*
*/
class StepComponent  : public juce::Component,
                       public juce::ChangeListener
{
public:
    StepComponent(Step& s);
    ~StepComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void setSizeAndPos(int range);
    void syncWithStep();
    
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StepComponent)

    int shapeHeight = 30;

    const Step& step;
    
    std::vector<std::unique_ptr<NoteComponent>> noteComponents;


};
