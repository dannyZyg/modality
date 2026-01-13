#pragma once

#include "Data/Cursor.h"
#include "juce_core/juce_core.h"
#include <JuceHeader.h>
#include <functional>

// Shortcut data structure to hold keybinding information
struct Shortcut
{
    juce::KeyPress keyPress;
    std::vector<Mode> applicableModes;
    std::function<bool()> action;
    juce::String shortDescription;
    juce::String longDescription;

    Shortcut (const juce::KeyPress& key,
              const std::vector<Mode>& modes,
              std::function<bool()> callback,
              const juce::String& shortDesc,
              const juce::String& longDesc)
        : keyPress (key), applicableModes (modes), action (callback), shortDescription (shortDesc), longDescription (longDesc) {}

    // Helper method to check if this shortcut applies to a specific mode
    bool appliesTo (Mode mode) const
    {
        return applicableModes.empty() || std::find (applicableModes.begin(), applicableModes.end(), mode) != applicableModes.end();
    }

    juce::String getShortcutKey()
    {
        return keyPress.getTextDescription().toLowerCase();
    }
};

class KeyboardShortcutManager
{
public:
    KeyboardShortcutManager();

    // Handle a key press for a specific mode
    bool handleKeyPress (const juce::KeyPress& key, Mode mode);

    // Add a shortcut to the manager
    void registerShortcut (Shortcut shortcut);

    void registerShortcuts (std::vector<Shortcut> shortcuts);

    // Remove a shortcut for a specific mode and key
    void deregisterShortcut (const juce::KeyPress& key, Mode mode);

    // Get all shortcuts for a specific mode
    std::vector<Shortcut> getShortcutsForMode (Mode mode) const;

    // Get description for a specific shortcut
    juce::String getShortcutDescription (const juce::KeyPress& key, Mode mode) const;

private:
    // Store shortcuts directly in a vector
    std::vector<Shortcut> activeShortcuts;
};
