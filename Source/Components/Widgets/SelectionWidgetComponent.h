#pragma once

#include "Components/Widgets/ISelectableWidget.h"
#include <JuceHeader.h>

struct SelectionOption
{
    juce::String displayText;
    juce::String value;
};

class SelectionWidgetComponent : public ISelectableWidget
{
public:
    SelectionWidgetComponent (const juce::String& t, std::vector<SelectionOption> opts, juce::Value valueToBindTo);

    SelectionWidgetComponent (const juce::String& t, std::vector<SelectionOption> opts, juce::String initialValue, std::function<void (const juce::String&)> onChanged);

    virtual ~SelectionWidgetComponent() override;

    void setup();

    bool keyPressed (const juce::KeyPress& key) override;
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    juce::String title;
    std::function<void (double)> selectionCallback;
    juce::Value selectedValue;

    std::vector<SelectionOption> options;
    size_t selectedIndex = 0;

    std::function<void (const juce::String&)> onChanged;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SelectionWidgetComponent)
};
