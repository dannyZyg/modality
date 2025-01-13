#pragma once

#include <JuceHeader.h>
#include "Step.h"
#include "Cursor.h"
#include "SequenceView.h"
#include "Sequence.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::AnimatedAppComponent
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void update() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;
    

    bool keyPressed (const juce::KeyPress& key) override;

private:
    //==============================================================================
    // Your private member variables go here...
    juce::String keyText = "";
    
    std::vector<std::unique_ptr<Step>> steps;
    
    Sequence sequence;
    Cursor cursor;
    SequenceView sequenceView;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
