#pragma once

#include "Data/Cursor.h"
#include "juce_gui_basics/juce_gui_basics.h"

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
