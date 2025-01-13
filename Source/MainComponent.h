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
    
    enum class Mode { normal, visual };
    constexpr const char* ModeToString(Mode m) throw();

private:
    //==============================================================================
    // Your private member variables go here...
    int selectedPortion = 0;
    int numPortions = 10;
    juce::String keyText = "";
    
    std::vector<std::unique_ptr<Step>> steps;
    std::vector<std::unique_ptr<Step>> visualSelection;
    
    Cursor cursor;
    Sequence sequence{};
    
    Mode mode = Mode::normal;

    void resizeSteps();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
