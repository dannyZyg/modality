#include "Components/Settings/SequenceSettingsManager.h"
#include "Audio/MidiOutputManager.h"
#include "Components/Settings/MidiSettingsSelectionFactory.h"
#include "Components/Settings/PaginatedSettingsComponent.h"
#include "Components/Widgets/ISelectableWidget.h"
#include "Components/Widgets/SelectionWidgetComponent.h"
#include "juce_core/juce_core.h"

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
}

SequenceSettingsManager::~SequenceSettingsManager() {}

MenuNode* SequenceSettingsManager::getMenuNodeRoot()
{
    return menuRoot.get();
}

void SequenceSettingsManager::buildMidiSettingsMenu()
{
    std::vector<std::unique_ptr<ISelectableWidget>> widgets;

    auto devices = midiOutManager.getAvailableDevices();
    std::vector<SelectionOption> deviceOptions;
    std::vector<SelectionOption> channelOptions;

    for (auto d : devices)
    {
        deviceOptions.push_back (SelectionOption { d.name, d.identifier });
    }

    for (int i = 1; i < 17; ++i)
    {
        channelOptions.push_back (SelectionOption { "Channel " + String (i), String (i) });
    }

    auto& seq = cursor.getSelectedSequence();

    widgets.push_back (std::make_unique<SelectionWidgetComponent> ("MIDI Output", deviceOptions, seq.getMidiOutputIdAsValue()));
    widgets.push_back (std::make_unique<SelectionWidgetComponent> ("MIDI Channel", channelOptions, seq.getMidiOutputChannelAsValue()));

    auto midiSettings = std::make_unique<PaginatedSettingsComponent> (std::move (widgets));

    midiSettingsNode->setComponent (std::move (midiSettings));
}
