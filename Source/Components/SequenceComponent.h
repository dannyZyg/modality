/*
  ==============================================================================

    SequenceComponent.h
    Created: 13 Jan 2025 3:17:50pm
    Author:  Danny Keig

  ==============================================================================
*/

#pragma once

#include "Data/Cursor.h"
#include <JuceHeader.h>

// Forward declaration
class Transport;

//==============================================================================
/*
*/
class SequenceComponent : public juce::Component
{
public:
    SequenceComponent (const Cursor& c, const Transport& t);
    ~SequenceComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void update();
    void setCurrentPlayheadTime (double time);
    juce::Path createNotePath (Note& n);

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SequenceComponent)

    const Cursor& cursor;
    const Transport& transport;
    double currentPlayheadTime_ = 0.0;
};
