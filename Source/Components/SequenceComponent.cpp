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
    /* auto& seq = cursor.getSequence(0); */
    /* for (auto& step : seq.steps) { */
    /*     auto stepComponent = std::make_unique<StepComponent>(*step); */
    /*     addAndMakeVisible(stepComponent.get()); */
    /*     stepComponents.emplace_back(std::move(stepComponent)); */
    /* } */
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

        g.setColour (juce::Colours::orange);
        g.fillPath(tri);
        g.setColour (juce::Colours::black);
        g.strokePath(tri, juce::PathStrokeType(1.0f));
        /* g.drawRect(rect, 2.0f); */
        /* g.fillRect(rect); */

    }

}

juce::Path SequenceComponent::createNotePath(Note& n)
{


    auto point = CoordinateUtils::musicToScreen(n, getWidth(), getHeight(), cursor.timeline, cursor.scale);

    float x = point.x + 25;
    float y = point.y;

    juce::Path path;
    path.startNewSubPath (juce::Point<float> (x, y));
    path.lineTo (juce::Point<float> (x + x, 25));
    path.lineTo (juce::Point<float> (x, 25));
    path.closeSubPath();
    return path;
}

void SequenceComponent::resized()
{
}

//==============================================================================
void SequenceComponent::update()
{
    // This function is called at the frequency specified by the setFramesPerSecond() call
    // in the constructor. You can use it to update counters, animate values, etc.

    /* for (size_t i = 0; i < stepComponents.size(); i++) { */
    /*     stepComponents[i]->setNoteComponentBounds(visibleRange); */
    /* } */

    //auto usedDegreeRange = cursor.getSequence(0).getUsedDegreeRange();
    int verticalBounds = calculateNoteComponentVerticalBounds();

    for (size_t i = 0; i < stepComponents.size(); i++) {
        stepComponents[i]->setNoteComponentBounds(verticalBounds);
    }

}


juce::Point<float> SequenceComponent::calculateStepPositions(size_t index, float blockSize)
{
    /* for (size_t i = 0; i < stepComponents.size(); i++) { */
    /*     stepComponents[i]->setSizeAndPos(visibleRange); */
    /* } */

    return juce::Point<float>{1.0, 1.0};
}

int SequenceComponent::calculateNoteComponentVerticalBounds()
{
  // Using the range from the lowest degree used to the highest degree used
  // calculate the vertical height each note component can use.
  int defaultRange = 24;  // -12 to 12 as an example
  auto degreeRange = cursor.getSequence(0).getUsedDegreeRange();
  int absRange = std::abs(degreeRange.second - degreeRange.first);

  int higherDegreeRange = std::max(absRange, defaultRange);

  return getHeight() / higherDegreeRange;
}
