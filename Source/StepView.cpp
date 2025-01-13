/*
  ==============================================================================

    StepView.cpp
    Created: 13 Jan 2025 3:18:41pm
    Author:  Danny Keig

  ==============================================================================
*/

#include <JuceHeader.h>
#include "StepView.h"

//==============================================================================
StepView::StepView(const Step& s) : step(s)
{
}

StepView::~StepView()
{
}

void StepView::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::white);   // clear the background
    g.setColour (juce::Colours::black);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component
    
    juce::Rectangle<int> bar (pos.x, pos.y, shapeHeight, shapeHeight);
    
    if (step.isSelected()) {
        g.setColour (juce::Colours::black);
        g.fillRect(bar);
    } else if (false) {
        g.setColour (juce::Colours::yellow);
        g.fillRect(bar);
        g.setColour (juce::Colours::black);
        g.drawRect(bar);
    } else {
        g.setColour (juce::Colours::lightgrey);
        g.fillRect(bar);
        g.setColour (juce::Colours::black);
        g.drawRect(bar);
    }

}

void StepView::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}
