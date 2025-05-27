#pragma once
#include "Components/Widgets/ISelectableWidget.h"
#include <JuceHeader.h>

class SliderWidgetComponent : public ISelectableWidget, public juce::Slider::Listener
{
public:
    //==============================================================================
    SliderWidgetComponent();
    SliderWidgetComponent (double _min, double _max, double _initial, juce::String _title);
    virtual ~SliderWidgetComponent() override;

    void setup();

    // Component overrides
    bool keyPressed (const juce::KeyPress& key) override;
    //void focusGained (FocusChangeType cause) override;
    void paint (juce::Graphics&) override;
    void resized() override;
    void sliderValueChanged (juce::Slider* slider) override;

private:
    juce::Slider horizontalSlider;
    double min = 0.0;
    double max = 100.0;
    double interval = 1.0;
    double initial = 50.0;

    juce::String title = "slider";

    std::function<void (double)> sliderCallback;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SliderWidgetComponent)
};
