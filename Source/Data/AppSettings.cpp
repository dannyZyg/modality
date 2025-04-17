#include "Data/AppSettings.h"

AppSettings& AppSettings::getInstance()
{
    static AppSettings instance;
    return instance;
}

AppSettings::AppSettings()
    : appProperties(std::make_unique<juce::ApplicationProperties>())
{
}

AppSettings::~AppSettings()
{
    // Properties pointer is owned by appProperties, don't delete it
    // appProperties will be properly destroyed by unique_ptr
}

void AppSettings::initialise(const juce::String& applicationName)
{
    if (isInitialised)
        return;

    juce::PropertiesFile::Options options;
    options.applicationName = applicationName;
    options.filenameSuffix = ".settings";
    options.osxLibrarySubFolder = "Application Support";
    options.folderName = applicationName;
    options.storageFormat = juce::PropertiesFile::storeAsXML;

    appProperties->setStorageParameters(options);
    properties = appProperties->getUserSettings();

    isInitialised = true;
}

void AppSettings::shutdown()
{
    if (properties != nullptr)
    {
        properties->saveIfNeeded();
    }

    // Reset properties pointer before destroying appProperties
    properties = nullptr;
    appProperties.reset();
    isInitialised = false;
}

bool AppSettings::getBoolValue(const juce::String& keyName, bool defaultValue)
{
    return properties->getBoolValue(keyName, defaultValue);
}

void AppSettings::setBoolValue(const juce::String& keyName, bool value)
{
    properties->setValue(keyName, value);
    properties->saveIfNeeded();
}

int AppSettings::getIntValue(const juce::String& keyName, int defaultValue)
{
    return properties->getIntValue(keyName, defaultValue);
}

void AppSettings::setIntValue(const juce::String& keyName, int value)
{
    properties->setValue(keyName, value);
    properties->saveIfNeeded();
}

double AppSettings::getDoubleValue(const juce::String& keyName, double defaultValue)
{
    return properties->getDoubleValue(keyName, defaultValue);
}

void AppSettings::setDoubleValue(const juce::String& keyName, double value)
{
    properties->setValue(keyName, value);
    properties->saveIfNeeded();
}

juce::String AppSettings::getStringValue(const juce::String& keyName, const juce::String& defaultValue)
{
    return properties->getValue(keyName, defaultValue);
}

void AppSettings::setStringValue(const juce::String& keyName, const juce::String& value)
{
    properties->setValue(keyName, value);
    properties->saveIfNeeded();
}

// Application-specific settings

int AppSettings::getLastWindowWidth()
{
    return getIntValue("window.lastWidth", 800);
}

void AppSettings::setLastWindowWidth(int width)
{
    setIntValue("window.lastWidth", width);
}

int AppSettings::getLastWindowHeight()
{
    return getIntValue("window.lastHeight", 600);
}

void AppSettings::setLastWindowHeight(int height)
{
    setIntValue("window.lastHeight", height);
}
