/*
  ==============================================================================

    NoteComponent.cpp
    Created: 17 Jan 2025 1:50:00pm
    Author:  Danny Keig

  ==============================================================================
*/

#include <JuceHeader.h>
#include "NoteComponent.h"

//==============================================================================
NoteComponent::NoteComponent(const Note& n) : note(n) {}

NoteComponent::~NoteComponent()
{
}

void NoteComponent::paint (juce::Graphics& g)
{
    //g.setColour(Colours::purple);
    //g.drawRect(getLocalBounds(), 1);

    float timeInSeconds = juce::Time::getMillisecondCounterHiRes() / 500.0f;
    float sineValue = std::sin(timeInSeconds * juce::MathConstants<float>::pi); // Oscillates between -1 and 1

    // Map sineValue to a 0 to 1 range
    float blendFactor = (sineValue + 1.0f) * 0.5f;
    // Interpolate between black and light grey
    juce::Colour blink = juce::Colours::black.interpolatedWith(juce::Colours::lightgrey, blendFactor);

    juce::Path path = createPath();

    if (note.isSelected()) {
        g.setColour (blink);
        g.fillPath(path);
        g.setColour (juce::Colours::black);
        g.strokePath(path, juce::PathStrokeType(1.0f));
    // TODO: fix visual select
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

void NoteComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

void NoteComponent::setSizeAndPos(int range)
{
    float stepSize = getHeight() / range;
    pos.x = getWidth() / 2;
    pos.y = (getHeight() / 2) - (stepSize / 2) + (stepSize * note.getDegree());
    shapeHeight = stepSize;
}

juce::Path NoteComponent::createPath()
{
    float x = static_cast<float>(getWidth()) / 2;
    float y = static_cast<float>(getHeight());

    juce::Path path;
    path.startNewSubPath (juce::Point<float> (x, 0));
    path.lineTo (juce::Point<float> (x + x, getHeight() / 2));
    path.lineTo (juce::Point<float> (x, getHeight()));
    path.closeSubPath();
    return path;
}
