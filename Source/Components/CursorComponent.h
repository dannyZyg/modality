#pragma once

#include <JuceHeader.h>
#include "Data/Cursor.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class CursorComponent : public juce::Component
{
public:
    //==============================================================================
    CursorComponent(const Cursor& c);
    ~CursorComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void update();

private:
    const Cursor& cursor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CursorComponent)
};
