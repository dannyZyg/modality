/*
  ==============================================================================

    SequenceComponent.cpp
    Created: 13 Jan 2025 3:17:50pm
    Author:  Danny Keig

  ==============================================================================
*/

#include "SequenceComponent.h"
#include "Audio/Transport.h"
#include "AppColours.h"
#include "CoordinateUtils.h"
#include <JuceHeader.h>

//==============================================================================
SequenceComponent::SequenceComponent (const Cursor& c, const Transport& t)
    : cursor (c), transport (t)
{
    setWantsKeyboardFocus (false);
}

SequenceComponent::~SequenceComponent()
{
}

void SequenceComponent::paint (juce::Graphics& g)
{
    float width = static_cast<float> (getWidth());
    float height = static_cast<float> (getHeight());

    // Draw the playhead
    g.setColour (AppColours::playhead);

    const auto& selectedSequence = cursor.getSelectedSequence();
    double tempoBPM = transport.getTempo();
    double secondsPerBeat = 60.0 / tempoBPM;
    double sequenceDurationInSeconds = selectedSequence.lengthBeats * secondsPerBeat;

    if (sequenceDurationInSeconds > 0)
    {
        double loopedPosition = std::fmod (currentPlayheadTime_, sequenceDurationInSeconds);
        double playheadX = (loopedPosition / sequenceDurationInSeconds) * width;
        g.drawLine (static_cast<float> (playheadX), 0, static_cast<float> (playheadX), height, 3.0f);
    }

    // Draw the outline
    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);

    // Draw the notes
    for (auto& note : cursor.getSelectedSequence().notes)
    {
        auto tri = CoordinateUtils::getTriangleAtPoint (*note, width, height, cursor.getCurrentTimeline(), cursor.getCurrentScale());

        if (note->hasAnyModifier())
        {
            g.setColour (juce::Colours::orange);
        }
        else
        {
            g.setColour (juce::Colours::lightgrey);
        }

        g.fillPath (tri);
        g.setColour (juce::Colours::black);
        g.strokePath (tri, juce::PathStrokeType (1.0f));
    }
}

void SequenceComponent::resized()
{
}

//==============================================================================
void SequenceComponent::update()
{
}

void SequenceComponent::setCurrentPlayheadTime (double time)
{
    currentPlayheadTime_ = time;
    repaint();
}
