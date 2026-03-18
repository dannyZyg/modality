#include "Components/Widgets/SelectionWidgetComponent.h"

SelectionWidgetComponent::SelectionWidgetComponent (const juce::String& t, std::vector<SelectionOption> opts, juce::Value valueToBindTo) : title (t), options (opts)
{
    selectedValue.referTo (valueToBindTo);

    for (size_t i = 0; i < options.size(); ++i)
    {
        if (options[i].value == selectedValue.toString())
        {
            selectedIndex = i;
        }
    }
    setWantsKeyboardFocus (true);
}

SelectionWidgetComponent::SelectionWidgetComponent (const juce::String& t, std::vector<SelectionOption> opts, juce::String initialValue, std::function<void (const juce::String&)> callback) : title (t), options (opts), onChanged (callback)
{
    for (size_t i = 0; i < options.size(); ++i)
    {
        if (options[i].value == initialValue)
        {
            selectedIndex = i;
        }
    }
    setWantsKeyboardFocus (true);
}

SelectionWidgetComponent::~SelectionWidgetComponent() {}

void SelectionWidgetComponent::setup()
{
}

bool SelectionWidgetComponent::keyPressed (const juce::KeyPress& key)
{
    if (key == juce::KeyPress::createFromDescription ("h"))
    {
        selectedIndex = static_cast<size_t> (juce::jmax (int (selectedIndex) - 1, 0));

        if (onChanged)
            onChanged (options[selectedIndex].value);
        else
            selectedValue = options[selectedIndex].value;

        return true;
    }

    if (key == juce::KeyPress::createFromDescription ("l"))
    {
        selectedIndex = static_cast<size_t> (juce::jmin (int (selectedIndex) + 1, int (options.size() - 1)));

        if (onChanged)
            onChanged (options[selectedIndex].value);
        else
            selectedValue = options[selectedIndex].value;

        return true;
    }

    return false;
}

void SelectionWidgetComponent::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    g.setColour (juce::Colours::white);
    g.setFont (15.0f);

    auto titleHeight = juce::jmin (30, bounds.getHeight() / 3);
    auto titleArea = bounds.removeFromTop (titleHeight);

    g.drawText (title, titleArea, juce::Justification::centred, true);

    auto valueArea = bounds.reduced (5);

    if (isSelected())
    {
        auto leftArrowArea = valueArea.removeFromLeft (20);
        auto rightArrowArea = valueArea.removeFromRight (20);

        if (selectedIndex > 0)
        {
            juce::Path leftArrow;
            leftArrow.addTriangle (
                leftArrowArea.getX(), leftArrowArea.getCentreY(), leftArrowArea.getRight(), leftArrowArea.getY(), leftArrowArea.getRight(), leftArrowArea.getBottom());

            g.fillPath (leftArrow);
        }

        if (selectedIndex < options.size() - 1)
        {
            juce::Path rightArrow;
            rightArrow.addTriangle (
                rightArrowArea.getRight(), rightArrowArea.getCentreY(), rightArrowArea.getX(), rightArrowArea.getY(), rightArrowArea.getX(), rightArrowArea.getBottom());
            g.fillPath (rightArrow);
        }
    }

    g.drawText (options[selectedIndex].displayText, valueArea, juce::Justification::centred, true);

    if (isSelected())
    {
        g.setColour (juce::Colours::aqua);

        float borderThickness = 2.0f;
        g.drawRect (getLocalBounds().toFloat(), borderThickness);
    }
}

void SelectionWidgetComponent::resized()
{
}
