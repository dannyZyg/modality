/*
  ==============================================================================

    SequenceComponent.cpp
    Created: 13 Jan 2025 3:17:50pm
    Author:  Danny Keig

  ==============================================================================
*/

#include <JuceHeader.h>
#include "SequenceComponent.h"
#include "CoordinateUtils.h"

//==============================================================================
SequenceComponent::SequenceComponent(const Cursor& c)
    : cursor(c)
{
    setWantsKeyboardFocus(false);
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

    for (auto& note : cursor.getSelectedSequence().notes) {

        auto tri = CoordinateUtils::getTriangleAtPoint(*note, getWidth(), getHeight(), cursor.timeline, cursor.scale);

        if (note->hasAnyModifier()) {
            g.setColour (juce::Colours::orange);
        } else {
            g.setColour (juce::Colours::lightgrey);
        }

        g.fillPath(tri);
        g.setColour (juce::Colours::black);
        g.strokePath(tri, juce::PathStrokeType(1.0f));
        /* g.drawRect(rect, 2.0f); */
        /* g.fillRect(rect); */
    }
}

void SequenceComponent::resized()
{
}

//==============================================================================
void SequenceComponent::update()
{
}
