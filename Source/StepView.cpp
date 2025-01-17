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
    for (const auto& note : step.notes) {
        note->pos.x = getWidth() / 2;
        note->pos.y = getWidth() / 2;
    }
}

StepView::~StepView()
{
}

juce::Path StepView::createPath(juce::Point<float> p)
{
    juce::Path path;
    path.startNewSubPath (juce::Point<float> (p.x, p.y));
    path.lineTo (juce::Point<float> (p.x + shapeHeight, p.y + shapeHeight / 2));
    path.lineTo (juce::Point<float> (p.x, p.y + shapeHeight));
    path.closeSubPath();
    return path;
}

void StepView::paint (juce::Graphics& g)
{
    //g.fillAll (juce::Colours::white);   // clear the background
    //g.drawRect (getLocalBounds(), 1);   // draw an outline around the component
    
    float timeInSeconds = juce::Time::getMillisecondCounterHiRes() / 500.0f;
    float sineValue = std::sin(timeInSeconds * juce::MathConstants<float>::pi); // Oscillates between -1 and 1
    
    // Map sineValue to a 0 to 1 range
    float blendFactor = (sineValue + 1.0f) * 0.5f;
    // Interpolate between black and light grey
    juce::Colour blink = juce::Colours::black.interpolatedWith(juce::Colours::lightgrey, blendFactor);
    
    
    if(step.notes.size() == 0) {
        juce::PathStrokeType strokeType(0.5f); // Stroke width of 3.0 pixels

//        juce::Path path = createPath(pos);
//        
//        // Apply the dashed pattern to the stroke
//        float dashLengths[] = {5.0f, 5.0f}; // 5 pixels on, 5 pixels off
//        int numDashLengths = 2; // Number of elements in the dashLengths array
//       
//        strokeType.createDashedStroke(path, path, dashLengths, numDashLengths);
//        g.setColour(juce::Colours::black);
//        g.strokePath(path, strokeType);
//        
//        g.setColour (blink);
//        g.fillPath(path);
        
        return;
    }

    
    for (int i = 0; i < step.notes.size(); i++) {
        g.setColour (juce::Colours::black);
        
        int x = step.notes[i]->pos.x;
        int y = step.notes[i]->pos.y;

        juce::Line<float> line (juce::Point<float> (x, y),
                                juce::Point<float> (getWidth() / 2, getHeight() / 2));
        g.drawLine (line, 1.0f);

        juce::Path path;
        path.startNewSubPath (juce::Point<float> (x, y));
        path.lineTo (juce::Point<float> (x + shapeHeight, y + shapeHeight / 2));
        path.lineTo (juce::Point<float> (x, y + shapeHeight));
        path.closeSubPath();
        
        if (step.isSelected() && step.notes[i]->isSelected()) {
            g.setColour (blink);
            g.fillPath(path);
            g.setColour (juce::Colours::black);
            g.strokePath(path, juce::PathStrokeType(1.0f));
        // TODO: fix visual select
        } else if (step.isSelected()) {
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


}

void StepView::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

void StepView::setSizeAndPos(int range)
{
    for (const auto& note: step.notes) {
        float stepSize = getHeight() / range;
        note->pos.x = getWidth() / 2;
        note->pos.y = (getHeight() / 2) - (stepSize / 2) + (stepSize * step.stepValue);
        shapeHeight = stepSize;
    }
}

