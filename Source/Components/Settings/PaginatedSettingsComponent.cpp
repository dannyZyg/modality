#include "Components/Settings/PaginatedSettingsComponent.h"
#include "juce_gui_basics/juce_gui_basics.h"

PaginatedSettingsComponent::PaginatedSettingsComponent (std::vector<std::unique_ptr<ISelectableWidget>> w, juce::String description)
    : widgets (std::move (w)), descriptionText (std::move (description))
{
    setWantsKeyboardFocus (true);
    addAndMakeVisible (label);

    for (auto& widget : widgets)
    {
        addAndMakeVisible (widget.get());
    }

    // After all widgets are added, set the first one as selected if available
    if (! widgets.empty())
    {
        selectedWidgetIndex = 0;
        widgets[selectedWidgetIndex]->setSelected (true);
    }
}

PaginatedSettingsComponent::~PaginatedSettingsComponent()
{
}

void PaginatedSettingsComponent::paint (juce::Graphics& g)
{
    if (descriptionText.isNotEmpty())
    {
        auto descX = 20;
        auto descWidth = getWidth() - (2 * descX);
        g.setColour (juce::Colours::lightgrey);
        g.setFont (juce::Font (juce::FontOptions (14.0f)));
        g.drawFittedText (descriptionText, descX, 0, descWidth, descriptionHeight, juce::Justification::centredLeft, 2);
    }

    if (widgets.empty())
        return;

    // Scroll indicators
    auto widgetAreaTop = 20 + (descriptionText.isNotEmpty() ? descriptionHeight : 0);
    auto widgetAreaBottom = getHeight() - footerHeight;
    int visible = visibleWidgetCount();
    g.setColour (juce::Colours::lightgrey.withAlpha (0.8f));
    g.setFont (juce::Font (juce::FontOptions (14.0f)));
    if (scrollOffset > 0)
        g.drawText (juce::CharPointer_UTF8 ("\xe2\x96\xb2"), getLocalBounds().getX(), widgetAreaTop - 16, getWidth(), 14, juce::Justification::centred, false);
    if (scrollOffset + visible < static_cast<int> (widgets.size()))
    {
        auto lastWidgetBottom = widgetAreaTop + visible * 90 - 5;
        g.drawText (juce::CharPointer_UTF8 ("\xe2\x96\xbc"), getLocalBounds().getX(), lastWidgetBottom, getWidth(), 14, juce::Justification::centred, false);
    }

    auto& selected = widgets[selectedWidgetIndex];
    if (! selected->isSelected())
        return;

    auto footerBounds = getLocalBounds().removeFromBottom (footerHeight);
    auto leftInset = 50;
    auto rowHeight = 20;
    auto footerRowWidth = footerBounds.getWidth() - (2 * leftInset);
    auto footerX = footerBounds.getX() + leftInset;

    g.setColour (juce::Colours::lightgrey);
    g.setFont (juce::Font (juce::FontOptions (16.0f).withStyle ("Italic")));

    // widget-specific hints
    auto hints = selected->getShortcutHints();

    if (! hints.empty())
    {
        auto buildLine = [] (const std::vector<ISelectableWidget::ShortcutHint>& slice) -> juce::String
        {
            juce::String s;
            for (const auto& [key, desc] : slice)
            {
                if (s.isNotEmpty())
                    s += "   ";
                s += key + ": " + desc;
            }
            return s;
        };

        auto font = g.getCurrentFont();
        juce::String fullLine = buildLine (hints);

        juce::GlyphArrangement ga;
        ga.addLineOfText (font, fullLine, 0.0f, 0.0f);
        auto textWidth = static_cast<int> (ga.getBoundingBox (0, -1, true).getWidth());

        if (textWidth <= footerRowWidth)
        {
            g.drawText (fullLine, footerX, footerBounds.getY() + 5, footerRowWidth, rowHeight, juce::Justification::left, true);
        }
        else
        {
            // Split hints at the midpoint and render two rows
            auto mid = hints.size() / 2;
            std::vector<ISelectableWidget::ShortcutHint> firstHalf (hints.begin(), hints.begin() + static_cast<int> (mid));
            std::vector<ISelectableWidget::ShortcutHint> secondHalf (hints.begin() + static_cast<int> (mid), hints.end());

            g.drawText (buildLine (firstHalf), footerX, footerBounds.getY() + 5, footerRowWidth, rowHeight, juce::Justification::left, true);
            g.drawText (buildLine (secondHalf), footerX, footerBounds.getY() + 25, footerRowWidth, rowHeight, juce::Justification::left, true);
        }
    }

    g.drawText ("j/k: navigate   d: remove", footerX, footerBounds.getY() + 48, footerRowWidth, rowHeight, juce::Justification::left, true);
}

void PaginatedSettingsComponent::resized()
{
    label.setBounds (getLocalBounds());

    auto x = 20;
    auto y = 20 + (descriptionText.isNotEmpty() ? descriptionHeight : 0);
    auto width = getWidth() - (2 * x);
    auto height = 85;
    auto padding = 90;
    auto availableHeight = getHeight() - footerHeight;

    for (int i = 0; i < static_cast<int> (widgets.size()); ++i)
    {
        if (i < scrollOffset)
        {
            widgets[static_cast<size_t> (i)]->setBounds (0, 0, 0, 0); // hide above scroll window
            continue;
        }

        if (y + height > availableHeight)
        {
            widgets[static_cast<size_t> (i)]->setBounds (0, 0, 0, 0); // hide below scroll window
            continue;
        }

        widgets[static_cast<size_t> (i)]->setBounds (x, y, width, height);
        y += padding;
    }
}

int PaginatedSettingsComponent::visibleWidgetCount() const
{
    auto height = 85;
    auto padding = 90;
    auto y = 20 + (descriptionText.isNotEmpty() ? descriptionHeight : 0);
    auto availableHeight = getHeight() - footerHeight;
    int count = 0;
    while (y + height <= availableHeight)
    {
        ++count;
        y += padding;
    }
    return juce::jmax (1, count);
}

void PaginatedSettingsComponent::update() {}

void PaginatedSettingsComponent::focusGained (FocusChangeType)
{
    if (! widgets.empty())
        widgets[selectedWidgetIndex]->grabKeyboardFocus();
}

bool PaginatedSettingsComponent::keyPressed (const juce::KeyPress& key)
{
    if (widgets.empty())
        return false;

    if (key == juce::KeyPress::createFromDescription ("d"))
    {
        if (onRemove)
            onRemove();
        return true;
    }

    if (key == juce::KeyPress::createFromDescription ("j"))
    {
        widgets[selectedWidgetIndex]->setSelected (false);
        selectedWidgetIndex = (selectedWidgetIndex + 1) % widgets.size();
        widgets[selectedWidgetIndex]->setSelected (true);

        int visible = visibleWidgetCount();
        if (static_cast<int> (selectedWidgetIndex) >= scrollOffset + visible)
            scrollOffset = static_cast<int> (selectedWidgetIndex) - visible + 1;
        if (static_cast<int> (selectedWidgetIndex) < scrollOffset)
            scrollOffset = static_cast<int> (selectedWidgetIndex);

        resized();
        widgets[selectedWidgetIndex]->grabKeyboardFocus();
        repaint();
        return true;
    }

    if (key == juce::KeyPress::createFromDescription ("k"))
    {
        widgets[selectedWidgetIndex]->setSelected (false);
        selectedWidgetIndex = (selectedWidgetIndex - 1 + widgets.size()) % widgets.size();
        widgets[selectedWidgetIndex]->setSelected (true);

        int visible = visibleWidgetCount();
        if (static_cast<int> (selectedWidgetIndex) < scrollOffset)
            scrollOffset = static_cast<int> (selectedWidgetIndex);
        if (static_cast<int> (selectedWidgetIndex) >= scrollOffset + visible)
            scrollOffset = static_cast<int> (selectedWidgetIndex) - visible + 1;

        resized();
        widgets[selectedWidgetIndex]->grabKeyboardFocus();
        repaint();
        return true;
    }

    return false;
}
