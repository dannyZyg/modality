#include "Components/Settings/PaginatedSettingsComponent.h"
#include "juce_gui_basics/juce_gui_basics.h"

PaginatedSettingsComponent::PaginatedSettingsComponent (std::vector<std::unique_ptr<ISelectableWidget>> w) : widgets (std::move (w))
{
    addAndMakeVisible (label);

    for (auto& widget : widgets)
    {
        addAndMakeVisible (widget.get());
    }

    // After all widgets are added, set the first one as selected if available
    if (! widgets.empty()) // Check if vector is not empty
    {
        selectedWidgetIndex = 0; // Set to 0 to select the first widget
        widgets[selectedWidgetIndex]->setSelected (true);
    }
}

PaginatedSettingsComponent::~PaginatedSettingsComponent()
{
}

void PaginatedSettingsComponent::paint (juce::Graphics& g) {}

void PaginatedSettingsComponent::resized()
{
    label.setBounds (getLocalBounds());

    auto x = 20;
    auto y = 20;
    auto width = getWidth() - (2 * x);
    auto height = 65;
    auto padding = 70;

    for (auto& widget : widgets)
    {
        widget->setBounds (x, y, width, height);
        y += padding;
    }
}

void PaginatedSettingsComponent::update() {}

bool PaginatedSettingsComponent::keyPressed (const juce::KeyPress& key)
{
    if (widgets.empty())
        return false;

    if (key == juce::KeyPress::createFromDescription ("j"))
    {
        widgets[selectedWidgetIndex]->setSelected (false);
        selectedWidgetIndex = (selectedWidgetIndex + 1) % widgets.size();
        widgets[selectedWidgetIndex]->setSelected (true);
        widgets[selectedWidgetIndex]->grabKeyboardFocus();
        return true;
    }

    if (key == juce::KeyPress::createFromDescription ("k"))
    {
        widgets[selectedWidgetIndex]->setSelected (false);
        selectedWidgetIndex = (selectedWidgetIndex - 1 + widgets.size()) % widgets.size();
        widgets[selectedWidgetIndex]->setSelected (true);
        widgets[selectedWidgetIndex]->grabKeyboardFocus();
        return true;
    }

    return false;
}
