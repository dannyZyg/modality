#pragma once

#include <JuceHeader.h>

class AppSettings
{
public:
    static AppSettings& getInstance();

    void initialise(const juce::String& applicationName);
    void shutdown();

    template<typename T>
    T getValue(const juce::String& keyName, const T& defaultValue);

    template<typename T>
    void setValue(const juce::String& keyName, const T& value);

    bool getBoolValue(const juce::String& keyName, bool defaultValue = false);
    void setBoolValue(const juce::String& keyName, bool value);

    int getIntValue(const juce::String& keyName, int defaultValue = 0);
    void setIntValue(const juce::String& keyName, int value);

    double getDoubleValue(const juce::String& keyName, double defaultValue = 0.0);
    void setDoubleValue(const juce::String& keyName, double value);

    juce::String getStringValue(const juce::String& keyName, const juce::String& defaultValue = "");
    void setStringValue(const juce::String& keyName, const juce::String& value);

    int getLastWindowWidth();
    void setLastWindowWidth(int width);

    int getLastWindowHeight();
    void setLastWindowHeight(int height);

private:
    AppSettings();
    ~AppSettings();

    std::unique_ptr<juce::ApplicationProperties> appProperties;
    juce::PropertiesFile* properties = nullptr; // Not owned by this class

    bool isInitialised = false;

    // Private constructor and destructor to enforce singleton pattern
    AppSettings(const AppSettings&) = delete;
    AppSettings& operator=(const AppSettings&) = delete;
};

// Template implementations need to be visible to all clients
template<typename T>
T AppSettings::getValue(const juce::String& keyName, const T& defaultValue)
{
    return static_cast<T>(properties->getValue(keyName, juce::String(defaultValue)));
}

template<typename T>
void AppSettings::setValue(const juce::String& keyName, const T& value)
{
    properties->setValue(keyName, juce::String(value));
    properties->saveIfNeeded();
}
