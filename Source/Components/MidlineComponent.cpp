#include "MidlineComponent.h"
#include "CoordinateUtils.h"
#include <JuceHeader.h>

//==============================================================================
MidlineComponent::MidlineComponent (const Cursor& c)
    : cursor (c)
{
}

MidlineComponent::~MidlineComponent() {}

void MidlineComponent::paint (juce::Graphics& g)
{
    float width = static_cast<float> (getWidth());
    float height = static_cast<float> (getHeight());

    // Draw the sequence midline (pitch = degree 0)
    auto x = CoordinateUtils::timeToScreenX (0.0, width, cursor.getCurrentTimeline());
    auto y = CoordinateUtils::degreeToScreenY (0.0, height, cursor.getCurrentScale());
    auto base = juce::Rectangle<float> (x, y, width, CoordinateUtils::getStepHeight (height, cursor.getCurrentScale()));

    g.setColour (juce::Colours::lightgrey.withLightness (0.9f));
    g.fillRect (base);
    g.setColour (juce::Colours::black.withLightness (0.7f));
    g.drawRect (base, 1.0f);
}

void MidlineComponent::resized()
{
}

//==============================================================================
void MidlineComponent::update()
{
}
