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

    /**
     * Get a MIDI output by device identifier.
     * Opens the device if not already open, caches it for future use.
     *
     * @param deviceId The MIDI device identifier. Empty string returns default output.
     * @return Pointer to the MIDI output, or nullptr if device not found/failed to open.
     */
    juce::MidiOutput* getOutput (const juce::String& deviceId);

    /**
     * Get the default MIDI output (first available device).
     * This is equivalent to calling getOutput("").
     *
     * @return Pointer to the default MIDI output, or nullptr if no devices available.
     */
    juce::MidiOutput* getDefaultOutput();

    /**
     * Get a list of available MIDI output devices.
     *
     * @return Array of MIDI device info structs.
     */
    juce::Array<juce::MidiDeviceInfo> getAvailableDevices() const;

    /**
     * Check if a device identifier is valid (device exists).
     *
     * @param deviceId The device identifier to check.
     * @return true if device exists, false otherwise.
     */
    bool isDeviceAvailable (const juce::String& deviceId) const;

    /**
     * Send all-notes-off message to all open MIDI outputs.
     * Call this when stopping playback to prevent stuck notes.
     */
    void sendAllNotesOff();

    /**
     * Send all-notes-off to a specific output.
     *
     * @param output The MIDI output to send to (can be nullptr, will be ignored).
     */
    static void sendAllNotesOff (juce::MidiOutput* output);

    /**
     * Close all open MIDI outputs.
     * Call this on shutdown.
     */
    void closeAll();

    /**
     * Refresh the list of available devices.
     * Call this if devices may have been connected/disconnected.
     */
    void refreshDevices();

    /**
     * Get the device ID that is currently set as default.
     */
    const juce::String& getDefaultDeviceId() const;

    /**
     * Set which device should be used as the default.
     *
     * @param deviceId The device identifier to use as default.
     */
    void setDefaultDeviceId (const juce::String& deviceId);

private:
    // Map of device ID -> open MIDI output
    std::map<juce::String, std::unique_ptr<juce::MidiOutput>> openOutputs;

    // The default device ID (empty means use first available)
    juce::String defaultDeviceId;

    // Cached list of available devices
    juce::Array<juce::MidiDeviceInfo> availableDevices;

    // Lock for thread-safe access
    juce::CriticalSection lock;

    // Find device info by ID
    const juce::MidiDeviceInfo* findDeviceById (const juce::String& deviceId) const;

    // Open a device and add to cache
    juce::MidiOutput* openAndCacheDevice (const juce::String& deviceId);
};
