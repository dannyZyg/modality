#pragma once

#include "Data/Cursor.h"
#include <JuceHeader.h>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class CursorComponent : public juce::Component
{
public:
    //==============================================================================
    CursorComponent (const Cursor& c);
    ~CursorComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void update();
    void triggerYankFlash();

private:
    const Cursor& cursor;
    double yankFlashStartMs = -1.0;
    static constexpr double yankFlashDurationMs = 500.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CursorComponent)
};
