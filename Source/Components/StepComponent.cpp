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
    noteComponents.clear(); // Clear old components

    // TODO: Fix this so that z index ordering is not based on vector index
    for (auto& note : step.notes) {
        auto noteComponent = std::make_unique<NoteComponent>(*note);
        addAndMakeVisible(noteComponent.get());
        noteComponents.emplace_back(std::move(noteComponent));
    }
    repaint();
}


void StepComponent::paint (juce::Graphics& g)
{

    float timeInSeconds = juce::Time::getMillisecondCounterHiRes() / 500.0f;
    float sineValue = std::sin(timeInSeconds * juce::MathConstants<float>::pi); // Oscillates between -1 and 1

    // Map sineValue to a 0 to 1 range
    float blendFactor = (sineValue + 1.0f) * 0.5f;
    // Interpolate between black and light grey
    juce::Colour blink = juce::Colours::black.interpolatedWith(juce::Colours::lightgrey, blendFactor);

    if (step.isSelected()) {
        g.setColour(Colours::green);
        g.drawRect(getLocalBounds(), 1);
    }

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
    float x = getWidth() / 2;

    //juce::Logger::writeToLog("note components size: " + juce::String(noteComponents.size()));

    if (noteComponents.size() > 0) {
        g.setColour (juce::Colours::black);
        juce::Line<float> line (juce::Point<float> (x, maxDegreeYpos), juce::Point<float> (x, minDegreeYpos));
        g.drawLine (line, 1.0f);
    }
}

void StepComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
}

float StepComponent::mapDegreeToVerticalPosition(int degree, int minDegree, int maxDegree)
{
    float totalSteps = static_cast<float>(std::abs(maxDegree - minDegree)) + 1;
    float min = static_cast<float>(minDegree);
    float max = static_cast<float>(maxDegree);

    // Measure distance from the max value (maxDegree - degree)
    // Normalize within the total range
    // Scale to total steps
    return ((max - static_cast<float>(degree)) / (max - min)) * (totalSteps - 1);
}

void StepComponent::setNoteComponentBounds(int height)
{
    int min = -12;
    int max = 12;
    minDegreeYpos = getHeight() / 2;
    maxDegreeYpos = getHeight() / 2;

    for (const auto& noteComponent: noteComponents) {
        int degree = noteComponent->note.getDegree();

        float mappedPosition = mapDegreeToVerticalPosition(degree, min, max);

        int xPos = 0;
        int yPos = static_cast<int>(mappedPosition) * height;

        if (yPos < minDegreeYpos)
            minDegreeYpos = yPos;

        if (yPos > maxDegreeYpos)
            maxDegreeYpos = yPos;

        noteComponent->setBounds(xPos, yPos, getWidth(), height);
    }
}

void StepComponent::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if (source == &step) {
        syncWithStep();
    }
}


