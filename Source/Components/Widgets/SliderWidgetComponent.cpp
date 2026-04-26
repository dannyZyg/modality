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

SliderWidgetComponent::SliderWidgetComponent (const juce::String& _title, juce::Value valueToBindTo, double _min, double _max, double _interval)
    : min (_min), max (_max), interval (_interval), initial (static_cast<double> (valueToBindTo.getValue())), title (_title)
{
    setup();
    horizontalSlider.getValueObject().referTo (valueToBindTo);
}

void SliderWidgetComponent::setRange (double _min, double _max, double _interval)
{
    min = _min;
    max = _max;
    interval = _interval;
    horizontalSlider.setRange (min, max, interval);
}

void SliderWidgetComponent::setup()
{
    horizontalSlider.setSliderStyle (juce::Slider::SliderStyle::LinearHorizontal);
    horizontalSlider.setRange (min, max, interval);
    horizontalSlider.setValue (initial);
    horizontalSlider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    horizontalSlider.addListener (this);
    setWantsKeyboardFocus (true);
    horizontalSlider.setWantsKeyboardFocus (false);
    addAndMakeVisible (horizontalSlider);
}

SliderWidgetComponent::~SliderWidgetComponent() {}

void SliderWidgetComponent::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    auto inner = getLocalBounds().reduced (6);

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawText (title, inner.removeFromTop (22), juce::Justification::centred, true);

    g.setColour (isSelected() ? juce::Colours::aqua : juce::Colours::white);
    g.drawText (juce::String (horizontalSlider.getValue(), 2),
                inner.removeFromBottom (20),
                juce::Justification::centred,
                true);

    if (isSelected())
    {
        g.setColour (juce::Colours::aqua);
        g.drawRect (getLocalBounds().toFloat(), 2.0f);
    }
}

void SliderWidgetComponent::resized()
{
    horizontalSlider.setBounds (getLocalBounds().reduced (6));
}

std::vector<ISelectableWidget::ShortcutHint> SliderWidgetComponent::getShortcutHints() const
{
    return {
        { "h/l", "adjust" },
        { "m", "midpoint" },
        { "^", "min" },
        { "$", "max" },
    };
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
        repaint();
        if (sliderCallback)
        {
            sliderCallback (horizontalSlider.getValue());
        }
    }
}
