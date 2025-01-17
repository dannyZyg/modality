/*
  ==============================================================================

    SequenceComponent.h
    Created: 13 Jan 2025 3:17:50pm
    Author:  Danny Keig

  ==============================================================================
*/

#pragma once

#include "StepComponent.h"
#include "Cursor.h"
#include "Sequence.h"
#include <JuceHeader.h>

//==============================================================================
/*
*/
class SequenceComponent  : public juce::Component
{
public:
    SequenceComponent(const Sequence& s, const Cursor& c);
    ~SequenceComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void update();
    void resizeSteps();

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SequenceComponent)

    void drawSequence();
    void drawCursor();

    const Sequence& sequence;
    const Cursor& cursor;

    std::vector<std::unique_ptr<StepComponent>> stepComponents;

    int visibleRange = 30;

};
