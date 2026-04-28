#include "BeatLegendComponent.h"
#include "CoordinateUtils.h"

BeatLegendComponent::BeatLegendComponent (const Cursor& c) : cursor (c) {}

void BeatLegendComponent::paint (juce::Graphics& g)
{
    float width = static_cast<float> (getWidth());
    float height = static_cast<float> (getHeight());

    const auto& timeline = cursor.getCurrentTimeline();
    int upperBound = static_cast<int> (timeline.getUpperBound());

    g.setColour (juce::Colours::black.withAlpha (0.25f));

    for (int beat = 0; beat <= upperBound; ++beat)
    {
        float x = CoordinateUtils::timeToScreenX (static_cast<double> (beat), width, timeline);
        float y1 = height * 0.125f;
        float y2 = height * 0.875f;
        g.drawLine (x, y1, x, y2, 1.0f);
    }
}
