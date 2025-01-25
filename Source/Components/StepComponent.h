/*
  ==============================================================================

    StepComponent.h
    Created: 13 Jan 2025 3:18:41pm
    Author:  Danny Keig

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Components/NoteComponent.h"
#include "Data/Step.h"

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

    void setNoteComponentBounds(int height);
    void syncWithStep();

    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

    void calculateDegreeVisualRange();

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StepComponent)

    int shapeHeight = 30;

    const Step& step;

    std::vector<std::unique_ptr<NoteComponent>> noteComponents;

    float maxDegreeYpos = 0.0;
    float minDegreeYpos = 0.0;

    float mapDegreeToVerticalPosition(int degree, int minDegree, int maxDegree);

};
