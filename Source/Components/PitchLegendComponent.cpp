#include "PitchLegendComponent.h"
#include "CoordinateUtils.h"

static const std::vector<int> legendDegrees = { 12, 9, 6, 3, 0, -3, -6, -9, -12 };

PitchLegendComponent::PitchLegendComponent() {}

void PitchLegendComponent::paint (juce::Graphics& g)
{
    float height = static_cast<float> (getHeight());
    float width = static_cast<float> (getWidth());
    float stepH = CoordinateUtils::getStepHeight (height);

    g.setColour (juce::Colours::black);
    g.setFont (juce::Font (juce::FontOptions().withName ("Courier New").withHeight (11.0f)));

    for (int degree : legendDegrees)
    {
        float y = CoordinateUtils::degreeToScreenY (static_cast<double> (degree), height);
        juce::String label = (degree > 0 ? "+" : "") + juce::String (degree);
        g.drawText (label,
                    0,
                    static_cast<int> (y),
                    static_cast<int> (width),
                    static_cast<int> (stepH),
                    juce::Justification::centredRight,
                    false);
    }
}
