#pragma once

#include "Data/Cursor.h"
#include <JuceHeader.h>

class BeatLegendComponent : public juce::Component
{
public:
    explicit BeatLegendComponent (const Cursor& cursor);
    void paint (juce::Graphics& g) override;
    void resized() override {}

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BeatLegendComponent)

private:
    const Cursor& cursor;
};
