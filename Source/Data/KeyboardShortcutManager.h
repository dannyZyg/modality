#pragma once

#include <JuceHeader.h>
#include <functional>
#include <unordered_map>
#include "Data/Cursor.h"
#include <string>

/**
 * A class to manage keyboard shortcuts throughout the application.
 * This centralizes shortcut handling and allows for more flexible configurations.
 */
class KeyboardShortcutManager
{
public:
    // Action type - a function to be called when a shortcut is triggered
    using Action = std::function<bool()>;

    KeyboardShortcutManager();

    bool handleKeyPress(const juce::KeyPress& key, Mode mode);

    void addShortcut(const juce::KeyPress& key, Mode mode, Action action, const std::string& description = "");

    void addShortcut(const juce::KeyPress& key, std::vector<Mode> modes, Action action, const std::string& description = "");

    void removeShortcut(const juce::KeyPress& key, Mode mode);

    struct ShortcutInfo
    {
        juce::KeyPress keyPress;
        std::string description;
    };
    std::vector<ShortcutInfo> getShortcutsForMode(Mode mode) const;

    std::string getShortcutDescription(const juce::KeyPress& key, Mode mode) const;

private:
    std::unordered_map<Mode, std::unordered_map<int, std::pair<Action, std::string>>> shortcuts;

    int getKeyPressID(const juce::KeyPress& key) const;
};
