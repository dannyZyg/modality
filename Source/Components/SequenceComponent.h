/*
  ==============================================================================

    SequenceComponent.h
    Created: 13 Jan 2025 3:17:50pm
    Author:  Danny Keig

  ==============================================================================
*/

#pragma once

#include "Data/Composition.h"
#include "Data/Cursor.h"
#include <JuceHeader.h>

//==============================================================================
/*
*/
class SequenceComponent : public juce::Component
{
public:
    SequenceComponent (const Cursor& c, const Composition& composition);
    ~SequenceComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void update();
    void setCurrentPlayheadTime (double time);
    void setIsPlaying (bool playing);
    juce::Path createNotePath (Note& n);

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SequenceComponent)

    const Cursor& cursor;
    const Composition& composition;
    double currentPlayheadTime_ = 0.0;
    bool isPlaying_ = false;
};
