#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class ModifierMenuComponent : public juce::Component
{
public:
    //==============================================================================
    ModifierMenuComponent();
    ~ModifierMenuComponent() override;

    bool keyPressed (const juce::KeyPress& key) override;
    void focusGained(FocusChangeType cause) override;
    void focusLost(FocusChangeType cause) override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void update();

    void drawTopMode(juce::Graphics& g);
    void drawAddMode(juce::Graphics& g);
    void drawEditMode(juce::Graphics& g);
    void drawRemoveMode(juce::Graphics& g);

private:
    enum class ModMenuMode { top, edit, add, remove};

    ModMenuMode mode = ModMenuMode::top;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ModifierMenuComponent)
};
