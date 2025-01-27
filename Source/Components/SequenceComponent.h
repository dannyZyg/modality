/*
  ==============================================================================

    SequenceComponent.h
    Created: 13 Jan 2025 3:17:50pm
    Author:  Danny Keig

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Data/Cursor.h"
#include "Data/Sequence.h"
#include "Components/StepComponent.h"

//==============================================================================
/*
*/
class SequenceComponent  : public juce::Component
{
public:
    SequenceComponent(const Cursor& c);
    ~SequenceComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void update();

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SequenceComponent)

    void drawSequence();
    void drawCursor();

    const Cursor& cursor;

    std::vector<std::unique_ptr<StepComponent>> stepComponents;

    int visibleRange = 30;

    juce::Point<float> calculateStepPositions(size_t index, float blockSize);

    int calculateNoteComponentVerticalBounds();

};
