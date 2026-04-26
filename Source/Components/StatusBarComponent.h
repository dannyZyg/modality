#pragma once

#include "Data/Composition.h"
#include "Data/Cursor.h"
#include <JuceHeader.h>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class StatusBarComponent : public juce::Component
{
public:
    //==============================================================================
    StatusBarComponent (const Cursor& c, const Composition& comp);
    ~StatusBarComponent() override;

    void paint (juce::Graphics& g) override;
    void resized() override;
    void update();

    void setPiePercentage (float percentage);

private:
    int barHeight = 30;
    float piePercentage; // Value between 0 and 1

    const Cursor& cursor;
    const Composition& composition;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StatusBarComponent)
};
