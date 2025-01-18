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
StepComponent::StepComponent(Step& s) : step(s)
{
    s.addChangeListener(this);
    syncWithStep();
}

StepComponent::~StepComponent()
{
}

void StepComponent::syncWithStep() {
    juce::Logger::writeToLog("sync with Step called!");
    noteComponents.clear(); // Clear old components
    
    // TODO: Fix this so that z index ordering is not based on vector index
    for (size_t i = step.notes.size(); i-- > 0; ) {
        auto note = step.notes[i].get();
        if (note) {
            auto noteComponent = std::make_unique<NoteComponent>(*note);
            addAndMakeVisible(noteComponent.get());
            noteComponents.emplace_back(std::move(noteComponent));
        }
    }
    repaint();
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

    
    float maxY = getHeight() / 2;
    float minY = getHeight() / 2;

    for (const auto& noteComponent : noteComponents) {
        g.setColour (juce::Colours::black);
        
        if (noteComponent->pos.y > maxY)
            maxY = noteComponent->pos.y;
        
        if (noteComponent->pos.y < minY)
            minY = noteComponent->pos.y;

        //juce::Path path;
        //path.startNewSubPath (juce::Point<float> (x, y));
        //path.lineTo (juce::Point<float> (x + shapeHeight, y + shapeHeight / 2));
        //path.lineTo (juce::Point<float> (x, y + shapeHeight));
        //path.closeSubPath();
    }
    
    //TODO fix this so that pos x belongs to the step component (notes will share x, mostly)
    float x = noteComponents[0]->pos.x;
    
    if (noteComponents.size() > 0) {
        g.setColour (juce::Colours::black);
        juce::Line<float> line (juce::Point<float> (x, maxY), juce::Point<float> (x, minY));
        g.drawLine (line, 1.0f);
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

void StepComponent::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if (source == &step) {
        syncWithStep();
    }
}

