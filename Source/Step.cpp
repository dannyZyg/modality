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
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    
}

Step::~Step()
{
}

void Step::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
     draws some placeholder text to get you started.
     
     You should replace everything in this method with your own
     drawing code..
     */
    
    g.fillAll (juce::Colours::white);   // clear the background
    g.setColour (juce::Colours::black);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component
    
    juce::Rectangle<int> bar (pos.x, pos.y, shapeHeight, shapeHeight);
    
    if (isSelected) {
        g.setColour (juce::Colours::black);
        g.fillRect(bar);
    } else if (isVisuallySelected) {
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

void Step::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    
    // TODO: fix this to prevent shifting after the init
    // Currently resizes all the time
    pos.y = getHeight() / 2 - (shapeHeight / 2);
}

void Step::select() { isSelected = true; }

void Step::unselect() { isSelected = false; }

void Step::stepUp() { pos.y = pos.y - stepValue; }

void Step::stepDown() { pos.y = pos.y + stepValue; }
