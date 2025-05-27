#include "Components/Widgets/SliderWidgetComponent.h"
#include "juce_gui_basics/juce_gui_basics.h"

SliderWidgetComponent::SliderWidgetComponent()
{
    setup();
}

SliderWidgetComponent::SliderWidgetComponent (double _min, double _max, double _initial, juce::String _title)
    : min (_min), max (_max), initial (_initial), title (_title)
{
    setup();
}

void SliderWidgetComponent::setup()
{
    interval = max / 100.0;

    horizontalSlider.setSliderStyle (juce::Slider::SliderStyle::LinearHorizontal);
    horizontalSlider.setRange (min, max, interval);
    horizontalSlider.setValue (initial);
    //horizontalSlider.addListener (this);
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
        g.setColour (juce::Colours::aqua); // Or juce::Colours::red, juce::Colours::blue, etc.

        // Define border thickness
        float borderThickness = 2.0f; // Adjust as needed

        // Get the bounds of the component
        juce::Rectangle<int> bounds = getLocalBounds();

        // Option A: Square/Rectangular Border
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
        DBG ("Horizontal Slider Value: " << horizontalSlider.getValue());
        sliderCallback (horizontalSlider.getValue());
    }
}

void SliderWidgetComponent::resized()
{
    horizontalSlider.setBounds (getLocalBounds());
}
