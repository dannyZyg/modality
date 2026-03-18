#include "Components/Modifiers/ModifierMenuManager.h"
#include "Components/Modifiers/ModifierComponentFactory.h"
#include "Components/Settings/PaginatedSettingsComponent.h"
#include "Data/MenuNode.h"
#include "Data/ModifierRegistry.h"
#include <functional>
#include <memory>

ModifierMenuManager::ModifierMenuManager (Cursor& c, std::function<void (const juce::String&, int i)> s) : cursor (c), showMessage (s)
{
    buildMenu();
}

ModifierMenuManager::~ModifierMenuManager()
{
}

MenuNode* ModifierMenuManager::getMenuNodeRoot()
{
    return menuRoot.get();
}

std::function<void()> ModifierMenuManager::createCallback (ModifierType type, MenuNode* node)
{
    return [this, type, node]()
    {
        auto notes = cursor.findNotesForCursorMode();

        if (notes.empty())
        {
            showMessage ("No notes at cursor", 2000);
            return;
        }

        cursor.addModifier (type);

        auto& firstNote = *notes.front().get();
        auto modifier = firstNote.getModifier (type);

        if (! modifier.has_value())
            return;

        auto& state = modifier->getState();
        auto widgets = ModifierComponentFactory::createWidgets (type, state);

        auto component = std::make_unique<PaginatedSettingsComponent> (std::move (widgets));

        node->setComponent (std::move (component));
    };
}

void ModifierMenuManager::buildMenu()
{
    menuRoot = std::make_unique<MenuNode> ("Modifiers");

    auto& registry = ModifierRegistry::getInstance();
    auto types = registry.getRegisteredTypes();

    for (auto modifierType : types)
    {
        auto def = registry.getDefinition (modifierType);
        if (! def)
            continue;

        auto childNode = std::make_unique<MenuNode> (
            def->displayName,
            def->getNavShortcut());

        MenuNode* childPtr = childNode.get();
        childNode->onEnter = createCallback (modifierType, childPtr);

        menuRoot->addChild (std::move (childNode));
    }
}
