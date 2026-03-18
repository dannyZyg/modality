#pragma once

#include "Components/Widgets/ISelectableWidget.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include <JuceHeader.h>
#include <memory>
#include <vector>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class PaginatedSettingsComponent : public juce::Component
{
public:
    //==============================================================================
    PaginatedSettingsComponent (std::vector<std::unique_ptr<ISelectableWidget>> w);

    ~PaginatedSettingsComponent() override;

    bool keyPressed (const juce::KeyPress& key) override;
    /* void focusGained(FocusChangeType cause) override; */

    void paint (juce::Graphics&) override;
    void resized() override;
    void update();

private:
    juce::Label label;
    juce::Slider horizontalSlider;

    size_t selectedWidgetIndex = 0;

    std::vector<std::unique_ptr<ISelectableWidget>> widgets;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PaginatedSettingsComponent)
};
