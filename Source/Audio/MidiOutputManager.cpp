/*
  ==============================================================================

    MidiOutputManager.cpp
    Manages multiple MIDI output devices for per-track routing.

  ==============================================================================
*/

#include "MidiOutputManager.h"

MidiOutputManager::MidiOutputManager()
{
    refreshDevices();
}

MidiOutputManager::~MidiOutputManager()
{
    closeAll();
}

void MidiOutputManager::refreshDevices()
{
    juce::ScopedLock sl (lock);
    availableDevices = juce::MidiOutput::getAvailableDevices();
}

juce::Array<juce::MidiDeviceInfo> MidiOutputManager::getAvailableDevices() const
{
    juce::ScopedLock sl (lock);
    return availableDevices;
}

bool MidiOutputManager::isDeviceAvailable (const juce::String& deviceId) const
{
    juce::ScopedLock sl (lock);
    return findDeviceById (deviceId) != nullptr;
}

const juce::MidiDeviceInfo* MidiOutputManager::findDeviceById (const juce::String& deviceId) const
{
    for (const auto& device : availableDevices)
    {
        if (device.identifier == deviceId)
            return &device;
    }
    return nullptr;
}

juce::MidiOutput* MidiOutputManager::getOutput (const juce::String& deviceId)
{
    juce::ScopedLock sl (lock);

    // Empty deviceId means use default
    if (deviceId.isEmpty())
        return getDefaultOutput();

    // Check if already open
    auto it = openOutputs.find (deviceId);
    if (it != openOutputs.end() && it->second != nullptr)
        return it->second.get();

    // Try to open the device
    return openAndCacheDevice (deviceId);
}

juce::MidiOutput* MidiOutputManager::getDefaultOutput()
{
    juce::ScopedLock sl (lock);

    // If a default is explicitly set, use it
    if (defaultDeviceId.isNotEmpty())
    {
        auto it = openOutputs.find (defaultDeviceId);
        if (it != openOutputs.end() && it->second != nullptr)
            return it->second.get();

        return openAndCacheDevice (defaultDeviceId);
    }

    // Otherwise use first available device
    if (availableDevices.isEmpty())
    {
        refreshDevices();
        if (availableDevices.isEmpty())
            return nullptr;
    }

    const auto& firstDevice = availableDevices.getFirst();

    // Check if already open
    auto it = openOutputs.find (firstDevice.identifier);
    if (it != openOutputs.end() && it->second != nullptr)
        return it->second.get();

    return openAndCacheDevice (firstDevice.identifier);
}

juce::MidiOutput* MidiOutputManager::openAndCacheDevice (const juce::String& deviceId)
{
    // Find the device info
    const auto* deviceInfo = findDeviceById (deviceId);
    if (deviceInfo == nullptr)
    {
        // Device not found, try refreshing the list
        refreshDevices();
        deviceInfo = findDeviceById (deviceId);
        if (deviceInfo == nullptr)
        {
            juce::Logger::writeToLog ("MIDI output device not found: " + deviceId);
            return nullptr;
        }
    }

    // Open the device
    auto output = juce::MidiOutput::openDevice (deviceId);
    if (output == nullptr)
    {
        juce::Logger::writeToLog ("Failed to open MIDI output: " + deviceInfo->name);
        return nullptr;
    }

    juce::Logger::writeToLog ("Opened MIDI output: " + deviceInfo->name);

    // Cache and return
    auto* rawPtr = output.get();
    openOutputs[deviceId] = std::move (output);
    return rawPtr;
}

void MidiOutputManager::sendAllNotesOff()
{
    juce::ScopedLock sl (lock);

    for (auto& pair : openOutputs)
    {
        if (pair.second != nullptr)
            sendAllNotesOff (pair.second.get());
    }
}

void MidiOutputManager::sendAllNotesOff (juce::MidiOutput* output)
{
    if (output == nullptr)
        return;

    for (int channel = 1; channel <= 16; ++channel)
    {
        output->sendMessageNow (juce::MidiMessage::allNotesOff (channel));
        output->sendMessageNow (juce::MidiMessage::allSoundOff (channel));
    }
}

void MidiOutputManager::closeAll()
{
    juce::ScopedLock sl (lock);

    // Send all notes off before closing
    for (auto& pair : openOutputs)
    {
        if (pair.second != nullptr)
            sendAllNotesOff (pair.second.get());
    }

    openOutputs.clear();
    juce::Logger::writeToLog ("Closed all MIDI outputs");
}

const juce::String& MidiOutputManager::getDefaultDeviceId() const
{
    return defaultDeviceId;
}

void MidiOutputManager::setDefaultDeviceId (const juce::String& deviceId)
{
    juce::ScopedLock sl (lock);
    defaultDeviceId = deviceId;
}
