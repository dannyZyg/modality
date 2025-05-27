#pragma once
#include "Data/MenuNode.h"
#include <JuceHeader.h>
#include <functional>
#include <stack>
#include <vector>

// Forward declarations
class Cursor;
enum class Mode;

class ContextualMenuComponent : public juce::Component, public juce::Timer
{
public:
    //==============================================================================
    ContextualMenuComponent();
    virtual ~ContextualMenuComponent() override;

    // Add a single shortcut
    void addShortcut (const juce::KeyPress& key,
                      const std::vector<Mode>& modes,
                      std::function<bool()> action,
                      const juce::String& description);

    // Component overrides
    bool keyPressed (const juce::KeyPress& key) override;
    void focusGained (FocusChangeType cause) override;
    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

    // UI Helpers
    void displayMenu (MenuNode* rootNode);
    void showMessage (juce::String message, int timeoutMs = 2000);
    void navigateBack();
    void navigateTo (MenuNode* childNode);
    void closeMenu();

protected:
    juce::String currentTitle = "";
    juce::String currentMessage = "";

    Mode currentMode;

    // Draw methods that could be overridden by subclasses
    virtual void drawContextualContent (juce::Graphics& g);
    virtual void drawShortcutHelp (juce::Graphics& g);
    virtual void drawMessage (juce::Graphics& g);
    void drawSubNavOptions (juce::Graphics& g);

private:
    // Message timeout timer
    int messageTimeoutMs;

    MenuNode* currentMenuNode = nullptr;
    std::stack<MenuNode*> backStack; // To navigate back up the tree

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ContextualMenuComponent)
};
