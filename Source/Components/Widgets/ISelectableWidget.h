#pragma once
#include <JuceHeader.h>

/**
 * An abstract interface for any JUCE Component that can be selected,
 * providing a default implementation for selection state management.
 */
class ISelectableWidget : public juce::Component
{
public:
    virtual ~ISelectableWidget() override = default;

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
