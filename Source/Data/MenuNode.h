#pragma once

#include "juce_gui_basics/juce_gui_basics.h"
#include <JuceHeader.h>
#include <memory>

// Represents one level/page in the menu tree
struct MenuNode
{
    juce::String title;
    juce::KeyPress navShortcut;
    std::unique_ptr<juce::Component> component;
    std::vector<std::unique_ptr<MenuNode>> children;

    MenuNode (const juce::String& t) : title (t) {}

    MenuNode (const juce::String& t, const juce::KeyPress& k) : title (t), navShortcut (k) {}

    MenuNode (const juce::String& t, const juce::KeyPress& k, std::unique_ptr<juce::Component> c)
        : title (t), navShortcut (k), component (std::move (c)) {}

    MenuNode* addChild (std::unique_ptr<MenuNode> child);
};
