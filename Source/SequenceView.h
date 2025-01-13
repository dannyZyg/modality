/*
  ==============================================================================

    SequenceView.h
    Created: 13 Jan 2025 3:17:50pm
    Author:  Danny Keig

  ==============================================================================
*/

#pragma once

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

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SequenceView)
    
    void drawSequence();
    void drawCursor();
    
    const Sequence& sequence;
    const Cursor& cursor;
};
