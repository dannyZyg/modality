#include "Data/KeyboardShortcutManager.h"
#include "Data/Cursor.h"

KeyboardShortcutManager::KeyboardShortcutManager()
{
}

bool KeyboardShortcutManager::handleKeyPress(const juce::KeyPress& key, Mode mode)
{
    // First check context-specific shortcuts
    auto contextIter = shortcuts.find(mode);
    if (contextIter != shortcuts.end())
    {
        int keyID = getKeyPressID(key);
        auto& contextShortcuts = contextIter->second;
        auto shortcutIter = contextShortcuts.find(keyID);

        if (shortcutIter != contextShortcuts.end())
        {
            // Execute the action and return its result
            return shortcutIter->second.first();
        }
    }

    /* // If no context-specific shortcut found, check global shortcuts */
    /* if (currentContext != Context::Global) */
    /* { */
    /*     auto globalIter = shortcuts.find(Context::Global); */
    /*     if (globalIter != shortcuts.end()) */
    /*     { */
    /*         int keyID = getKeyPressID(key); */
    /*         auto& globalShortcuts = globalIter->second; */
    /*         auto shortcutIter = globalShortcuts.find(keyID); */
    /**/
    /*         if (shortcutIter != globalShortcuts.end()) */
    /*         { */
    /*             // Execute the global action */
    /*             return shortcutIter->second.first(); */
    /*         } */
    /*     } */
    /* } */

    // No shortcut found
    return false;
}

void KeyboardShortcutManager::addShortcut(
    const juce::KeyPress& key,
    Mode mode,
    Action action,
    const std::string& description)
{
    int keyID = getKeyPressID(key);
    shortcuts[mode][keyID] = std::make_pair(action, description);
}

void KeyboardShortcutManager::addShortcut(
    const juce::KeyPress& key,
    std::vector<Mode> modes,
    Action action,
    const std::string& description)
{
    for (auto mode : modes) {
        addShortcut(key, mode, action, description);
    }
}

void KeyboardShortcutManager::removeShortcut(
    const juce::KeyPress& key,
    Mode mode)
{
    auto contextIter = shortcuts.find(mode);
    if (contextIter != shortcuts.end())
    {
        int keyID = getKeyPressID(key);
        contextIter->second.erase(keyID);
    }
}

std::vector<KeyboardShortcutManager::ShortcutInfo>
KeyboardShortcutManager::getShortcutsForMode(Mode mode) const
{
    std::vector<ShortcutInfo> result;

    auto contextIter = shortcuts.find(mode);
    if (contextIter != shortcuts.end())
    {
        for (const auto& [keyID, actionPair] : contextIter->second)
        {
            juce::KeyPress keyPress(keyID);
            result.push_back({keyPress, actionPair.second});
        }
    }

    return result;
}

std::string KeyboardShortcutManager::getShortcutDescription(const juce::KeyPress& key, Mode mode) const
{
    // First check context-specific shortcuts
    auto contextIter = shortcuts.find(mode);
    if (contextIter != shortcuts.end())
    {
        int keyID = getKeyPressID(key);
        auto& contextShortcuts = contextIter->second;
        auto shortcutIter = contextShortcuts.find(keyID);

        if (shortcutIter != contextShortcuts.end())
        {
            return shortcutIter->second.second;
        }
    }

    /* // If not found, check global shortcuts */
    /* if (currentContext != Context::Global) */
    /* { */
    /*     auto globalIter = shortcuts.find(Context::Global); */
    /*     if (globalIter != shortcuts.end()) */
    /*     { */
    /*         int keyID = getKeyPressID(key); */
    /*         auto& globalShortcuts = globalIter->second; */
    /*         auto shortcutIter = globalShortcuts.find(keyID); */
    /**/
    /*         if (shortcutIter != globalShortcuts.end()) */
    /*         { */
    /*             return shortcutIter->second.second; */
    /*         } */
    /*     } */
    /* } */

    return ""; // No description found
}

int KeyboardShortcutManager::getKeyPressID(const juce::KeyPress& key) const
{
    return key.getKeyCode() + (key.getModifiers().getRawFlags() << 16);
}
