#pragma once

#include "Audio/MidiOutputManager.h"
#include "Components/Settings/PaginatedSettingsComponent.h"
#include "Components/Widgets/ISelectableWidget.h"
#include "Components/Widgets/SelectionWidgetComponent.h"
#include "Data/Cursor.h"
#include "Data/MenuNode.h"
#include "juce_core/juce_core.h"
#include <memory>

namespace MidiSettingsSelectionFactory
{

namespace detail
{
    const int numMidiChannels = 16;

    static std::vector<SelectionOption> buildMidiChannelOptions()
    {
        std::vector<SelectionOption> channelOptions;

        for (int i = 1; i <= numMidiChannels; ++i)
        {
            channelOptions.push_back (SelectionOption { "Channel " + String (i), String (i) });
        }
        return channelOptions;
    }

    static std::vector<SelectionOption> buildMidiDeviceOptions (const MidiOutputManager& midiOutManager)
    {
        auto devices = midiOutManager.getAvailableDevices();
        std::vector<SelectionOption> deviceOptions;

        for (auto d : devices)
        {
            deviceOptions.push_back (SelectionOption { d.name, d.identifier });
        }
        return deviceOptions;
    }

} // namespace detail
[[maybe_unused]] static std::unique_ptr<juce::Component> createComponent (const Cursor& cursor, const MidiOutputManager& midiOutManager)
{
    std::vector<std::unique_ptr<ISelectableWidget>> widgets;

    std::vector<SelectionOption> deviceOptions = detail::buildMidiDeviceOptions (midiOutManager);
    std::vector<SelectionOption> channelOptions = detail::buildMidiChannelOptions();

    auto& seq = cursor.getSelectedSequence();

    widgets.push_back (std::make_unique<SelectionWidgetComponent> ("MIDI Output", deviceOptions, seq.getMidiOutputIdAsValue()));
    widgets.push_back (std::make_unique<SelectionWidgetComponent> ("MIDI Channel", channelOptions, seq.getMidiOutputChannelAsValue()));

    auto midiSettings = std::make_unique<PaginatedSettingsComponent> (std::move (widgets));

    return midiSettings;
}

[[maybe_unused]] static std::unique_ptr<juce::Component> createComponent (
    const MidiOutputManager& midiOutManager,
    const juce::String& initialMidiOutput,
    const juce::String& initialMidiChannel,
    const std::function<void (const juce::String&)> onMidiOutputChanged,
    const std::function<void (const juce::String&)> onMidiChannelChanged)
{
    std::vector<std::unique_ptr<ISelectableWidget>> widgets;
    std::vector<SelectionOption> deviceOptions = detail::buildMidiDeviceOptions (midiOutManager);
    std::vector<SelectionOption> channelOptions = detail::buildMidiChannelOptions();

    widgets.push_back (std::make_unique<SelectionWidgetComponent> ("MIDI Output", deviceOptions, initialMidiOutput, onMidiOutputChanged));
    widgets.push_back (std::make_unique<SelectionWidgetComponent> ("MIDI Channel", channelOptions, initialMidiChannel, onMidiChannelChanged));

    auto midiSettings = std::make_unique<PaginatedSettingsComponent> (std::move (widgets));
    return midiSettings;
}

[[maybe_unused]] static std::unique_ptr<MenuNode> createMenuNode (const Cursor& cursor, const MidiOutputManager& midiOutManager)
{
    auto midiSettingsNode = std::make_unique<MenuNode> ("Midi Settings", juce::KeyPress::createFromDescription ("m"));
    auto settingsComponent = createComponent (cursor, midiOutManager);
    midiSettingsNode->setComponent (std::move (settingsComponent));

    return midiSettingsNode;
}

[[maybe_unused]] static std::unique_ptr<MenuNode> createMenuNode (
    const MidiOutputManager& midiOutManager,
    const juce::String& initialMidiOutput,
    const juce::String& initialMidiChannel,
    const std::function<void (const juce::String&)> onMidiOutputChanged,
    const std::function<void (const juce::String&)> onMidiChannelChanged)
{
    auto midiSettingsNode = std::make_unique<MenuNode> ("Midi Settings", juce::KeyPress::createFromDescription ("m"));

    auto settingsComponent = createComponent (midiOutManager, initialMidiOutput, initialMidiChannel, onMidiOutputChanged, onMidiChannelChanged);
    midiSettingsNode->setComponent (std::move (settingsComponent));

    return midiSettingsNode;
}

} // namespace MidiSettingsSelectionFactory
