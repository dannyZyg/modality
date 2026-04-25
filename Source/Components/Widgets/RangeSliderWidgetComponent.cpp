#include "Components/Widgets/RangeSliderWidgetComponent.h"

RangeSliderWidgetComponent::RangeSliderWidgetComponent (const juce::String& _title,
                                                        juce::Value minValueToBindTo,
                                                        juce::Value maxValueToBindTo,
                                                        double _min,
                                                        double _max,
                                                        double _interval)
    : min (_min), max (_max), interval (_interval), title (_title)
{
    setup();
    rangeSlider.getMinValueObject().referTo (minValueToBindTo);
    rangeSlider.getMaxValueObject().referTo (maxValueToBindTo);
}

RangeSliderWidgetComponent::~RangeSliderWidgetComponent() {}

void RangeSliderWidgetComponent::setup()
{
    rangeSlider.setSliderStyle (juce::Slider::SliderStyle::TwoValueHorizontal);
    rangeSlider.setRange (min, max, interval);
    rangeSlider.setMinValue (min);
    rangeSlider.setMaxValue (max);
    rangeSlider.addListener (this);
    rangeSlider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    setWantsKeyboardFocus (true);
    rangeSlider.setWantsKeyboardFocus (false);
    addAndMakeVisible (rangeSlider);
}

void RangeSliderWidgetComponent::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    auto inner = getLocalBounds().reduced (6);

    // --- Title ---
    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawText (title, inner.removeFromTop (22), juce::Justification::centred, true);

    // --- Numerical readout (bottom overlay) ---
    auto readoutArea = inner.removeFromBottom (20);
    int centreX = readoutArea.getCentreX();
    int gap = 16;

    juce::String minText = juce::String (rangeSlider.getMinValue(), 2);
    juce::String maxText = juce::String (rangeSlider.getMaxValue(), 2);

    auto minArea = juce::Rectangle<int> (readoutArea.getX(), readoutArea.getY(), centreX - gap - readoutArea.getX(), readoutArea.getHeight());
    auto maxArea = juce::Rectangle<int> (centreX + gap, readoutArea.getY(), readoutArea.getRight() - (centreX + gap), readoutArea.getHeight());

    g.setColour (activeHandle == ActiveHandle::Min ? juce::Colours::aqua : juce::Colours::white);
    g.drawText (minText, minArea, juce::Justification::centredRight, true);

    g.setColour (activeHandle == ActiveHandle::Max ? juce::Colours::aqua : juce::Colours::white);
    g.drawText (maxText, maxArea, juce::Justification::centredLeft, true);

    // --- Border ---
    if (isSelected())
    {
        g.setColour (juce::Colours::aqua);
        g.drawRect (getLocalBounds().toFloat(), 2.0f);
    }
}

void RangeSliderWidgetComponent::resized()
{
    rangeSlider.setBounds (getLocalBounds().reduced (6));
}

bool RangeSliderWidgetComponent::keyPressed (const juce::KeyPress& key)
{
    // Toggle active handle
    if (key == juce::KeyPress::createFromDescription ("o"))
    {
        activeHandle = (activeHandle == ActiveHandle::Min) ? ActiveHandle::Max : ActiveHandle::Min;
        repaint();
        return true;
    }

    // h/l act on whichever handle is active
    if (key == juce::KeyPress::createFromDescription ("h"))
    {
        if (activeHandle == ActiveHandle::Min)
            rangeSlider.setMinValue (std::max (rangeSlider.getMinValue() - interval, min));
        else
            rangeSlider.setMaxValue (std::max (rangeSlider.getMaxValue() - interval, rangeSlider.getMinValue()));
        return true;
    }

    if (key == juce::KeyPress::createFromDescription ("l"))
    {
        if (activeHandle == ActiveHandle::Min)
            rangeSlider.setMinValue (std::min (rangeSlider.getMinValue() + interval, rangeSlider.getMaxValue()));
        else
            rangeSlider.setMaxValue (std::min (rangeSlider.getMaxValue() + interval, max));
        return true;
    }

    if (key == juce::KeyPress::createFromDescription ("m"))
    {
        double mid = (min + max) / 2.0;
        rangeSlider.setMinValue (mid - interval);
        rangeSlider.setMaxValue (mid + interval);
        return true;
    }

    return false;
}

void RangeSliderWidgetComponent::sliderValueChanged (juce::Slider* /*slider*/)
{
    repaint(); // keep readout and marker in sync
}
