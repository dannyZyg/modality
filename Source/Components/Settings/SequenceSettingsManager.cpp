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
    auto sequenceProperties = std::make_unique<MenuNode> ("Sequence Properties", juce::KeyPress::createFromDescription ("p"));

    sequenceProperties->onEnter = [this]()
    {
        std::vector<std::unique_ptr<ISelectableWidget>> widgets;
        auto& seq = cursor.getSelectedSequence();

        // Sequence name
        widgets.push_back (std::make_unique<TextInputWidgetComponent> ("Sequence Name", seq.getNameAsValue(), 32));

        // Sequence scale
        auto onSelectScale = [&seq, this] (const juce::String& newScaleName)
        {
            midiOutManager.sendAllNotesOff();
            seq.setScale (newScaleName, cursor.getUndoManager());
            cursor.cursorPosition.yDegree = Degree (seq.getScale().getNearestDegree (cursor.cursorPosition.yDegree.value));
        };

        widgets.push_back (std::make_unique<SelectionWidgetComponent> ("Scale", getScaleOptions(), seq.getScale().getName(), onSelectScale));

        // Sequence timeline
        auto timelineSlider = std::make_unique<SliderWidgetComponent> ("Length", seq.getTimeline().getUpperBoundAsValue(), 1.0, 8.0, 0.25);
        widgets.push_back (std::move (timelineSlider));

        // Root note
        static const char* noteNames[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
        auto rootNoteFormatter = [] (double v) -> juce::String
        {
            int n = juce::jlimit (0, 127, static_cast<int> (v));
            return juce::String (n) + " \xe2\x80\x94 " + noteNames[n % 12] + juce::String (n / 12 - 1);
        };
        auto rootNoteSlider = std::make_unique<SliderWidgetComponent> ("Root Note", seq.getRootNoteAsValue(), 0.0, 127.0, 1.0, rootNoteFormatter);
        widgets.push_back (std::move (rootNoteSlider));

        auto settingsComponent = std::make_unique<PaginatedSettingsComponent> (std::move (widgets));
        propertiesNode->setComponent (std::move (settingsComponent));
    };

    propertiesNode = menuRoot->addChild (std::move (sequenceProperties));

    // Mute node
    auto muteNode = std::make_unique<MenuNode> ("Mute", juce::KeyPress ('m'));
    muteNode->tag = "muted";
    muteNode->onAction = [this]()
    {
        auto& seq = cursor.getSelectedSequence();
        bool currentlyMuted = seq.isMuted();

        // Mutually exclusive: unmute if already muted, otherwise mute and clear solo
        if (currentlyMuted)
        {
            seq.setMuted (false);
        }
        else
        {
            seq.setSoloed (false);
            seq.setMuted (true);
        }
    };
    menuRoot->addChild (std::move (muteNode));

    // Solo node
    auto soloNode = std::make_unique<MenuNode> ("Solo", juce::KeyPress ('s'));
    soloNode->tag = "soloed";
    soloNode->onAction = [this]()
    {
        auto& seq = cursor.getSelectedSequence();
        bool currentlySoloed = seq.isSoloed();

        // Mutually exclusive: unsolo if already soloed, otherwise solo and clear mute
        if (currentlySoloed)
        {
            seq.setSoloed (false);
        }
        else
        {
            seq.setMuted (false);
            seq.setSoloed (true);
        }
    };
    menuRoot->addChild (std::move (soloNode));
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
