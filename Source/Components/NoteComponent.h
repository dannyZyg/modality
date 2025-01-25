/*
  ==============================================================================

    NoteComponent.h
    Created: 17 Jan 2025 1:50:00pm
    Author:  Danny Keig

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Data/Note.h"

//==============================================================================
/*
*/
class NoteComponent  : public juce::Component
{
public:
    NoteComponent(const Note& n);
    ~NoteComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;


    const Note& note;

    void setSizeAndPos(int range);
    juce::Path createPath();

    juce::Point<float> pos{500, 500};
    int shapeHeight = 30;


private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NoteComponent)
};
