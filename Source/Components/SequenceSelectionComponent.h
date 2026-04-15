#pragma once

#include "Data/Composition.h"
#include "Data/Cursor.h"
#include <JuceHeader.h>

class SequenceSelectionComponent : public juce::Component, public juce::Value::Listener
{
public:
    //==============================================================================
    SequenceSelectionComponent (const Cursor& curs, Composition& comp);
    ~SequenceSelectionComponent() override;

    void paint (juce::Graphics& g) override;
    void resized() override;
    void update();

    void valueChanged (juce::Value& value) override;

private:
    const Cursor& cursor;
    Composition& composition;
    std::vector<juce::Value> sequenceNameValues;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SequenceSelectionComponent)
};
