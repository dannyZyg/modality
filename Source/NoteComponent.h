/*
  ==============================================================================

    NoteComponent.h
    Created: 17 Jan 2025 1:28:05pm
    Author:  Danny Keig

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class NoteComponent  : public juce::Component
{
public:
    NoteComponent();
    ~NoteComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NoteComponent)
};
