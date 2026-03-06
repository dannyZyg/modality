#pragma once

#include "Data/Composition.h"
#include "Data/Cursor.h"
#include <JuceHeader.h>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class SequenceSelectionComponent : public juce::Component
{
public:
    //==============================================================================
    SequenceSelectionComponent (const Cursor& curs, const Composition& comp);
    ~SequenceSelectionComponent() override;

    void paint (juce::Graphics& g) override;
    void resized() override;
    void update();

private:
    const Cursor& cursor;
    const Composition& composition;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SequenceSelectionComponent)
};
