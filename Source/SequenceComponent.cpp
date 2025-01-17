/*
  ==============================================================================

    SequenceComponent.cpp
    Created: 13 Jan 2025 3:17:50pm
    Author:  Danny Keig

  ==============================================================================
*/

#include <JuceHeader.h>
#include "SequenceComponent.h"

//==============================================================================
SequenceComponent::SequenceComponent(const Sequence& s, const Cursor& c)
    : sequence(s), cursor(c)
{
    for (int i = 0; i < s.steps.size(); ++i) {
        auto step = s.steps[i].get();
        if (step) {
            auto stepComponent = std::make_unique<StepComponent>(*step);
            addAndMakeVisible(stepComponent.get());
            stepComponents.emplace_back(std::move(stepComponent));
        }
    }
}

SequenceComponent::~SequenceComponent()
{
}

void SequenceComponent::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (juce::Colours::white);
}

void SequenceComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

void SequenceComponent::resizeSteps()
{
    int blockSize = getWidth() / (sequence.lengthBeats * sequence.stepsPerBeat);

    for (auto i = 0; i < stepComponents.size(); i++)
    {
        stepComponents[i]->setBounds(i * blockSize, 0, blockSize, getHeight());
    }
}

//==============================================================================
void SequenceComponent::update()
{
    // This function is called at the frequency specified by the setFramesPerSecond() call
    // in the constructor. You can use it to update counters, animate values, etc.

    for (int i = 0; i < stepComponents.size(); i++) {
        stepComponents[i]->setSizeAndPos(visibleRange);
    }
}
