#include "Data/KeyboardShortcutManager.h"
#include "Data/Cursor.h"

KeyboardShortcutManager::KeyboardShortcutManager()
{
}

bool KeyboardShortcutManager::handleKeyPress (const juce::KeyPress& key, Mode mode)
{
    // Search for a matching shortcut
    for (const auto& shortcut : activeShortcuts)
    {
        if (shortcut.keyPress == key && shortcut.appliesTo (mode))
        {
            // Execute the action and return its result
            return shortcut.action();
        }
    }

    // No shortcut found
    return false;
}

void KeyboardShortcutManager::registerShortcut (Shortcut shortcut)
{
    activeShortcuts.push_back (shortcut);
}

void KeyboardShortcutManager::registerShortcuts (std::vector<Shortcut> shortcuts)
{
    for (auto& shortcut : shortcuts)
    {
        registerShortcut (shortcut);
    }
}

void KeyboardShortcutManager::deregisterShortcut (const juce::KeyPress& key, Mode mode)
{
    // Remove shortcuts that match the given key and mode
    activeShortcuts.erase (
        std::remove_if (activeShortcuts.begin(), activeShortcuts.end(), [&key, &mode] (const Shortcut& shortcut)
                        { return shortcut.keyPress == key && shortcut.appliesTo (mode); }),
        activeShortcuts.end());
}

std::vector<Shortcut>
    KeyboardShortcutManager::getShortcutsForMode (Mode mode) const
{
    std::vector<Shortcut> result;

    // Find all shortcuts that apply to this mode
    for (const auto& shortcut : activeShortcuts)
    {
        if (shortcut.appliesTo (mode))
        {
            result.push_back (shortcut);
        }
    }

    return result;
}

juce::String KeyboardShortcutManager::getShortcutDescription (const juce::KeyPress& key, Mode mode) const
{
    // Find a matching shortcut
    for (const auto& shortcut : activeShortcuts)
    {
        if (shortcut.keyPress == key && shortcut.appliesTo (mode))
        {
            return shortcut.shortDescription;
        }
    }

    return ""; // No description found
}
