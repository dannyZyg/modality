#include "Components/Settings/SequenceSettingsManager.h"
#include "Audio/MidiOutputManager.h"
#include "Components/Settings/MidiSettingsSelectionFactory.h"
#include "Components/Settings/PaginatedSettingsComponent.h"
#include "Components/Widgets/ISelectableWidget.h"
#include "Components/Widgets/TextInputWidgetComponent.h"

SequenceSettingsManager::SequenceSettingsManager (Cursor& c, MidiOutputManager& m) : cursor (c), midiOutManager (m)
{
    menuRoot = std::make_unique<MenuNode> ("Sequence Settings");

    auto midiSettings = MidiSettingsSelectionFactory::createMenuNode (cursor, midiOutManager);

    midiSettings->onEnter = [this]()
    {
        auto component = MidiSettingsSelectionFactory::createComponent (cursor, midiOutManager);
        midiSettingsNode->setComponent (std::move (component));
    };

    midiSettingsNode = menuRoot->addChild (std::move (midiSettings));

    // Create name settings menu node
    auto nameSettings = std::make_unique<MenuNode> ("Name Settings", juce::KeyPress::createFromDescription ("n"));

    nameSettings->onEnter = [this]()
    {
        std::vector<std::unique_ptr<ISelectableWidget>> widgets;
        auto& seq = cursor.getSelectedSequence();

        widgets.push_back (std::make_unique<TextInputWidgetComponent> ("Sequence Name", seq.getNameAsValue(), 32));

        auto nameComponent = std::make_unique<PaginatedSettingsComponent> (std::move (widgets));
        nameSettingsNode->setComponent (std::move (nameComponent));
    };

    nameSettingsNode = menuRoot->addChild (std::move (nameSettings));
}

SequenceSettingsManager::~SequenceSettingsManager() {}

MenuNode* SequenceSettingsManager::getMenuNodeRoot()
{
    return menuRoot.get();
}
