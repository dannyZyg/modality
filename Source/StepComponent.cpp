/*
  ==============================================================================

    StepComponent.cpp
    Created: 13 Jan 2025 3:18:41pm
    Author:  Danny Keig

  ==============================================================================
*/

#include <JuceHeader.h>
#include "NoteComponent.h"
#include "StepComponent.h"

//==============================================================================
StepComponent::StepComponent(const Step& s) : step(s)
{
    for (int i = 0; i < s.notes.size(); ++i) {
        auto note = s.notes[i].get();
        if (note) {
            auto noteComponent = std::make_unique<NoteComponent>(*note);
            addAndMakeVisible(noteComponent.get());
            noteComponents.emplace_back(std::move(noteComponent));
        }
    }
}

StepComponent::~StepComponent()
{
}

void StepComponent::paint (juce::Graphics& g)
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


    for (const auto& noteComponent : noteComponents) {
        g.setColour (juce::Colours::black);

        //juce::Line<float> line (juce::Point<float> (x, y),
        //                        juce::Point<float> (getWidth() / 2, getHeight() / 2));
        //g.drawLine (line, 1.0f);

        //juce::Path path;
        //path.startNewSubPath (juce::Point<float> (x, y));
        //path.lineTo (juce::Point<float> (x + shapeHeight, y + shapeHeight / 2));
        //path.lineTo (juce::Point<float> (x, y + shapeHeight));
        //path.closeSubPath();
    }


}

void StepComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    
    for (auto i = 0; i < noteComponents.size(); i++)
    {
        noteComponents[i]->setBounds(0, 0, getWidth(), getHeight());
    }

}

void StepComponent::setSizeAndPos(int range)
{
    for (const auto& noteComponent: noteComponents) {
        noteComponent->setSizeAndPos(range);
    }
}

