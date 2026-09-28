#pragma once

#include "juce_graphics/juce_graphics.h"
#include "juce_gui_basics/juce_gui_basics.h"

class PitchLegendComponent : public juce::Component
{
public:
    PitchLegendComponent();
    void paint (juce::Graphics& g) override;
    void resized() override {}

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PitchLegendComponent)
};
