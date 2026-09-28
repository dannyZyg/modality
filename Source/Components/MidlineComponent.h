#pragma once

#include "Data/Cursor.h"
#include "juce_gui_basics/juce_gui_basics.h"

//==============================================================================
/*
*/
class MidlineComponent : public juce::Component
{
public:
    MidlineComponent (const Cursor& c);
    ~MidlineComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void update();

private:
    const Cursor& cursor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidlineComponent)
};
