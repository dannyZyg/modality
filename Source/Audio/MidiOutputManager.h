/*
  ==============================================================================

    MidiOutputManager.h
    Manages multiple MIDI output devices for per-track routing.

    Design:
    - Caches open MIDI outputs to avoid repeatedly opening/closing devices
    - Provides thread-safe access to output pointers
    - Handles device hot-plug gracefully (returns nullptr for disconnected devices)

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <map>

class MidiOutputManager
{
public:
    MidiOutputManager();
    ~MidiOutputManager();

    juce::MidiOutput* getOutput (const juce::String& deviceId);

    juce::MidiOutput* getDefaultOutput();

    juce::Array<juce::MidiDeviceInfo> getAvailableDevices() const;

    bool isDeviceAvailable (const juce::String& deviceId) const;

    void sendAllNotesOff();

    static void sendAllNotesOff (juce::MidiOutput* output);

    void closeAll();

    void refreshDevices();

    const juce::String& getDefaultDeviceId() const;

    void setDefaultDeviceId (const juce::String& deviceId);

private:
    std::map<juce::String, std::unique_ptr<juce::MidiOutput>> openOutputs;

    juce::String defaultDeviceId;

    juce::Array<juce::MidiDeviceInfo> availableDevices;

    juce::CriticalSection lock;

    const juce::MidiDeviceInfo* findDeviceById (const juce::String& deviceId) const;

    const juce::MidiDeviceInfo* findDeviceByName (const juce::String& deviceName) const;

    juce::MidiOutput* openAndCacheDevice (const juce::String& deviceId);
};
