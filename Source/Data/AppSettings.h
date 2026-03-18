#pragma once

#include "juce_core/juce_core.h"
#include <JuceHeader.h>

namespace AppSettingsIDs
{
#define DECLARE_ID(name) inline const juce::Identifier name { #name };

DECLARE_ID (WindowLastHeight)
DECLARE_ID (WindowLastWidth)
DECLARE_ID (MidiDefaultOutputDevice)
DECLARE_ID (MidiDefaultChannel)

#undef DECLARE_ID
} // namespace AppSettingsIDs

class AppSettings
{
public:
    static AppSettings& getInstance();

    void initialise (const juce::String& applicationName);
    void shutdown();

    template <typename T>
    T getValue (const juce::Identifier& keyName, const T& defaultValue);

    template <typename T>
    void setValue (const juce::Identifier& keyName, const T& value);

    bool getBoolValue (const juce::Identifier& keyName, bool defaultValue = false);
    void setBoolValue (const juce::Identifier& keyName, bool value);

    int getIntValue (const juce::Identifier& keyName, int defaultValue = 0);
    void setIntValue (const juce::Identifier& keyName, int value);

    double getDoubleValue (const juce::Identifier& keyName, double defaultValue = 0.0);
    void setDoubleValue (const juce::Identifier& keyName, double value);

    juce::String getStringValue (const juce::Identifier& keyName, const juce::String& defaultValue = "");
    void setStringValue (const juce::Identifier& keyName, const juce::String& value);

    int getLastWindowWidth();
    void setLastWindowWidth (int width);

    int getLastWindowHeight();
    void setLastWindowHeight (int height);

    juce::String getDefaultMidiOutputDevice();
    void setDefaultMidiOutputDevice (juce::String deviceID);

    juce::String getDefaultMidiChannel();
    void setDefaultMidiChannel (juce::String channel);

private:
    AppSettings();
    ~AppSettings();

    std::unique_ptr<juce::ApplicationProperties> appProperties;
    juce::PropertiesFile* properties = nullptr; // Not owned by this class

    bool isInitialised = false;

    // Private constructor and destructor to enforce singleton pattern
    AppSettings (const AppSettings&) = delete;
    AppSettings& operator= (const AppSettings&) = delete;
};

// Template implementations need to be visible to all clients
template <typename T>
T AppSettings::getValue (const juce::Identifier& keyName, const T& defaultValue)
{
    return static_cast<T> (properties->getValue (keyName, juce::String (defaultValue)));
}

template <typename T>
void AppSettings::setValue (const juce::Identifier& keyName, const T& value)
{
    properties->setValue (keyName, juce::String (value));
    properties->saveIfNeeded();
}
