#pragma once

#include "Data/Cursor.h"
#include "Data/MenuNode.h"
#include <memory>

class ModifierMenuManager
{
public:
    ModifierMenuManager (Cursor& cursor,
                         std::function<void (const juce::String&, int i)> showMessage,
                         std::function<void()> goBack);
    ~ModifierMenuManager();

    MenuNode* getMenuNodeRoot();

private:
    Cursor& cursor;
    std::function<void (const juce::String&, int i)> showMessage;
    std::function<void()> goBack;

    std::unique_ptr<MenuNode> menuRoot;

    std::function<void()> createCallback (ModifierType type, MenuNode* node);

    void buildMenu();
};
