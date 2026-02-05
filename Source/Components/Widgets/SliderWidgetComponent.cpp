#include "Components/Widgets/SliderWidgetComponent.h"

SliderWidgetComponent::SliderWidgetComponent()
{
    setup();
}

SliderWidgetComponent::SliderWidgetComponent (double _min, double _max, double _initial, juce::String _title)
    : min (_min), max (_max), initial (_initial), title (_title)
{
    setup();
}

SliderWidgetComponent::SliderWidgetComponent (const juce::String& _title, juce::Value valueToBindTo, double _min, double _max)
    : min (_min), max (_max), initial (static_cast<double> (valueToBindTo.getValue())), title (_title)
{
    setup();
    // Bind the slider's value object to the provided juce::Value
    // This creates two-way binding - changes to slider update ValueTree and vice versa
    horizontalSlider.getValueObject().referTo (valueToBindTo);
}

void SliderWidgetComponent::setup()
{
    interval = max / 100.0;

    horizontalSlider.setSliderStyle (juce::Slider::SliderStyle::LinearHorizontal);
    horizontalSlider.setRange (min, max, interval);
    horizontalSlider.setValue (initial);
    horizontalSlider.addListener (this);
    addAndMakeVisible (horizontalSlider);
}

SliderWidgetComponent::~SliderWidgetComponent() {}

void SliderWidgetComponent::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId)); // Background
    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawText (title, getLocalBounds().removeFromTop (30), juce::Justification::centred, true);

    if (isSelected())
    {
        g.setColour (juce::Colours::aqua);

        float borderThickness = 2.0f;
        juce::Rectangle<int> bounds = getLocalBounds();
        g.drawRect (bounds.toFloat(), borderThickness);
    }
}

bool SliderWidgetComponent::keyPressed (const juce::KeyPress& key)
{
    if (key == juce::KeyPress::createFromDescription ("h"))
    {
        double newSliderValue = horizontalSlider.getValue() - interval;
        horizontalSlider.setValue (std::max (newSliderValue, min));
    }

    if (key == juce::KeyPress::createFromDescription ("l"))
    {
        double newSliderValue = horizontalSlider.getValue() + interval;
        horizontalSlider.setValue (std::min (newSliderValue, max));
    }

    if (key == juce::KeyPress::createFromDescription ("m"))
    {
        double newSliderValue = max / 2;
        horizontalSlider.setValue (newSliderValue);
    }

    if (key == juce::KeyPress ('^', juce::ModifierKeys::shiftModifier, 0))
    {
        horizontalSlider.setValue (min);
    }

    if (key == juce::KeyPress ('$', juce::ModifierKeys::shiftModifier, 0))
    {
        horizontalSlider.setValue (max);
    }

    return false;
}

void SliderWidgetComponent::sliderValueChanged (juce::Slider* slider)
{
    if (slider == &horizontalSlider)
    {
        if (sliderCallback)
        {
            sliderCallback (horizontalSlider.getValue());
        }
    }
}

void SliderWidgetComponent::resized()
{
    horizontalSlider.setBounds (getLocalBounds());
}
