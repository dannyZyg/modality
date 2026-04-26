#pragma once
#include "Components/Widgets/ISelectableWidget.h"
#include <JuceHeader.h>

class RangeSliderWidgetComponent : public ISelectableWidget, public juce::Slider::Listener
{
public:
    // Value-bound constructor - binds min/max handles to two ValueTree properties
    RangeSliderWidgetComponent (const juce::String& title,
                                juce::Value minValueToBindTo,
                                juce::Value maxValueToBindTo,
                                double min,
                                double max,
                                double interval);

    virtual ~RangeSliderWidgetComponent() override;

    std::vector<ShortcutHint> getShortcutHints() const override;

    bool keyPressed (const juce::KeyPress& key) override;
    void paint (juce::Graphics&) override;
    void resized() override;
    void sliderValueChanged (juce::Slider* slider) override;

private:
    enum class ActiveHandle { Min, Max };

    void setup();

    juce::Slider rangeSlider;
    double min = 0.0;
    double max = 1.0;
    double interval = 0.01;

    juce::String title;
    ActiveHandle activeHandle = ActiveHandle::Min;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RangeSliderWidgetComponent)
};
