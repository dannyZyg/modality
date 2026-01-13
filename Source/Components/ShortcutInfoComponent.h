#pragma once

#include "Data/KeyboardShortcutManager.h"
#include <JuceHeader.h>

class ShortcutInfoComponent : public juce::Component
{
public:
    ShortcutInfoComponent (KeyboardShortcutManager& manager);

    ~ShortcutInfoComponent() override;

    bool keyPressed (const juce::KeyPress& key) override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void update();
    void nextPage();
    void prevPage();
    int getNumPages();
    int getNumShortcuts();

private:
    KeyboardShortcutManager& keyboardShortcutManager;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ShortcutInfoComponent)
    int currentPage = 0;
    int selectedIndex = 0;
    int numItemsPerPage = 10;

    void drawShortcutList (juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawSelectedInfo (juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawPageIndicators (juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawNavHelp (juce::Graphics& g, juce::Rectangle<int> bounds);
    void selectNext();
    void selectPrev();
};
