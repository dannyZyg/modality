#include "Components/Settings/SequenceSettingsManager.h"
#include "Audio/MidiOutputManager.h"
#include "Components/Settings/MidiSettingsSelectionFactory.h"
#include "Components/Settings/PaginatedSettingsComponent.h"
#include "Components/Widgets/ISelectableWidget.h"
#include "Components/Widgets/SelectionWidgetComponent.h"
#include "Components/Widgets/SliderWidgetComponent.h"
#include "Components/Widgets/TextInputWidgetComponent.h"
#include "Data/Scale.h"
#include <memory>

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
    auto sequenceProperties = std::make_unique<MenuNode> ("Sequence Properties", juce::KeyPress::createFromDescription ("s"));

    sequenceProperties->onEnter = [this]()
    {
        std::vector<std::unique_ptr<ISelectableWidget>> widgets;
        auto& seq = cursor.getSelectedSequence();

        // Sequence name
        widgets.push_back (std::make_unique<TextInputWidgetComponent> ("Sequence Name", seq.getNameAsValue(), 32));

        // Sequence scale
        auto onSelectScale = [&seq, this] (const juce::String& newScaleName)
        { midiOutManager.sendAllNotesOff(); seq.setScale (newScaleName, cursor.getUndoManager()); };

        widgets.push_back (std::make_unique<SelectionWidgetComponent> ("Scale", getScaleOptions(), seq.getScale().getName(), onSelectScale));

        // Sequence timeline
        auto timelineSlider = std::make_unique<SliderWidgetComponent> ("Length", seq.getTimeline().getUpperBoundAsValue(), 1.0, 8.0, 0.25);
        widgets.push_back (std::move (timelineSlider));

        auto settingsComponent = std::make_unique<PaginatedSettingsComponent> (std::move (widgets));
        propertiesNode->setComponent (std::move (settingsComponent));
    };

    propertiesNode = menuRoot->addChild (std::move (sequenceProperties));
}

SequenceSettingsManager::~SequenceSettingsManager() {}

MenuNode* SequenceSettingsManager::getMenuNodeRoot()
{
    return menuRoot.get();
}

std::vector<SelectionOption> SequenceSettingsManager::getScaleOptions()
{
    std::vector<SelectionOption> options;

    for (auto scale : scaleDefinitions)
    {
        options.push_back (SelectionOption (scale.first, scale.first));
    }
    return options;
}
