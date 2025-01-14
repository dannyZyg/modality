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
    pos.x = getWidth() / 2;
    pos.y = 0;
}

StepView::~StepView()
{
}

void StepView::paint (juce::Graphics& g)
{
    //g.fillAll (juce::Colours::white);   // clear the background
    //g.drawRect (getLocalBounds(), 1);   // draw an outline around the component
    g.setColour (juce::Colours::black);
    int triWidth = getWidth() / 2;
    
    
    juce::Line<float> line (juce::Point<float> (pos.x, pos.y),
                            juce::Point<float> (getWidth() / 2, getHeight() / 2));
    g.drawLine (line, 1.0f);

    juce::Path path;
    path.startNewSubPath (juce::Point<float> (pos.x, pos.y));
    path.lineTo (juce::Point<float> (pos.x + triWidth, pos.y + triWidth / 2));
    path.lineTo (juce::Point<float> (pos.x, pos.y + triWidth));
    path.closeSubPath();
    
    float timeInSeconds = juce::Time::getMillisecondCounterHiRes() / 500.0f;
    float sineValue = std::sin(timeInSeconds * juce::MathConstants<float>::pi); // Oscillates between -1 and 1
    
    // Map sineValue to a 0 to 1 range
    float blendFactor = (sineValue + 1.0f) * 0.5f;
    // Interpolate between black and light grey
    juce::Colour blink = juce::Colours::black.interpolatedWith(juce::Colours::lightgrey, blendFactor);

    if (step.isSelected()) {
        g.setColour (blink);
        g.fillPath(path);
        g.setColour (juce::Colours::black);
        g.strokePath(path, juce::PathStrokeType(1.0f));
    } else if (false) {
        g.setColour (juce::Colours::yellow);
        g.strokePath(path, juce::PathStrokeType(1.0f));
        g.setColour (juce::Colours::black);
        g.fillPath (path);
    } else {
        g.setColour (juce::Colours::white);
        g.fillPath(path);
        g.setColour (juce::Colours::black);
        g.strokePath(path, juce::PathStrokeType(1.0f));
    }

}

void StepView::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

void StepView::setPos(int range)
{
    float stepSize = getHeight() / range;
    pos.x = getWidth() / 2;
    pos.y = getHeight() / 2 + stepSize * step.stepValue;
}
