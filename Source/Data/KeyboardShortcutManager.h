#pragma once

#include <JuceHeader.h>
#include <functional>
#include "Data/Cursor.h"

// Shortcut data structure to hold keybinding information
struct Shortcut {
    juce::KeyPress keyPress;
    std::vector<Mode> applicableModes;
    std::function<bool()> action;
    juce::String description;

    Shortcut(const juce::KeyPress& key,
                   const std::vector<Mode>& modes,
                   std::function<bool()> callback,
                   const juce::String& desc)
        : keyPress(key), applicableModes(modes), action(callback), description(desc) {}

    // Helper method to check if this shortcut applies to a specific mode
    bool appliesTo(Mode mode) const {
        return applicableModes.empty() ||
               std::find(applicableModes.begin(), applicableModes.end(), mode) != applicableModes.end();
    }
};

class KeyboardShortcutManager
{
public:
    KeyboardShortcutManager();

    // Structure for returning shortcut info to the UI
    struct ShortcutInfo {
        juce::KeyPress keyPress;
        juce::String description;
    };

    // Handle a key press for a specific mode
    bool handleKeyPress(const juce::KeyPress& key, Mode mode);

    // Add a shortcut to the manager
    void registerShortcut(Shortcut shortcut);

    void registerShortcuts(std::vector<Shortcut> shortcuts);

    // Remove a shortcut for a specific mode and key
    void deregisterShortcut(const juce::KeyPress& key, Mode mode);

    // Get all shortcuts for a specific mode
    std::vector<ShortcutInfo> getShortcutsForMode(Mode mode) const;

    // Get description for a specific shortcut
    juce::String getShortcutDescription(const juce::KeyPress& key, Mode mode) const;

private:
    // Store shortcuts directly in a vector
    std::vector<Shortcut> activeShortcuts;
};
