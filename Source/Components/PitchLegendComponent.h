#pragma once

#include <JuceHeader.h>

class PitchLegendComponent : public juce::Component
{
public:
    PitchLegendComponent();
    void paint (juce::Graphics& g) override;
    void resized() override {}

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PitchLegendComponent)
};
