#pragma once
#include <JuceHeader.h>
#include <utility>
#include <vector>

/**
 * An abstract interface for any JUCE Component that can be selected,
 * providing a default implementation for selection state management.
 */
class ISelectableWidget : public juce::Component
{
public:
    /** A key/description pair used to render shortcut hint footers. */
    using ShortcutHint = std::pair<juce::String, juce::String>;

    virtual ~ISelectableWidget() override = default;

    /**
     * Returns the shortcuts that are active for this widget in its current state.
     * Override in subclasses to provide widget-specific hints.
     */
    virtual std::vector<ShortcutHint> getShortcutHints() const { return {}; }

    virtual void setSelected (bool selected)
    {
        if (isSelected_ != selected)
        {
            isSelected_ = selected;

            repaint();
        }
    }

    virtual bool isSelected() const
    {
        return isSelected_;
    }

protected:
    bool isSelected_ = false;
};
