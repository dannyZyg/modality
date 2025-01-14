/*
  ==============================================================================

    SequenceView.h
    Created: 13 Jan 2025 3:17:50pm
    Author:  Danny Keig

  ==============================================================================
*/

#pragma once

#include "StepView.h"
#include "Cursor.h"
#include "Sequence.h"
#include <JuceHeader.h>

//==============================================================================
/*
*/
class SequenceView  : public juce::Component
{
public:
    SequenceView(const Sequence& s, const Cursor& c);
    ~SequenceView() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void update();
    void resizeSteps();

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SequenceView)
    
    void drawSequence();
    void drawCursor();
    
    const Sequence& sequence;
    const Cursor& cursor;
    
    std::vector<std::unique_ptr<StepView>> stepViews;
    
    int visibleRange = 30;
    
};
