#pragma once

#include "Components/ContextualMenuComponent.h"
#include "Data/Cursor.h"
#include <JuceHeader.h>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class ModifierMenuComponent : public ContextualMenuComponent
{
public:
    //==============================================================================
    ModifierMenuComponent (Cursor& c);

    ~ModifierMenuComponent() override;

    /* bool keyPressed (const juce::KeyPress& key) override; */
    /* void focusGained(FocusChangeType cause) override; */

    /* void paint (juce::Graphics&) override; */
    /* void resized() override; */
    void update();

    void drawTopMode (juce::Graphics& g);
    void drawModifierOptions (juce::Graphics& g);
    void drawEditMode (juce::Graphics& g);
    void drawRemoveMode (juce::Graphics& g);

    /* void timerCallback() override; */

    /* void showMessage(juce::String message); */
    bool handleModifierChange (ModifierType t);

protected:
    // Override draw methods from ContextualMenu
    void drawContextualContent (juce::Graphics& g) override;

private:
    enum class ModMenuMode
    {
        top,
        edit,
        add,
        remove,
        complete
    };

    ModMenuMode mode = ModMenuMode::top;
    juce::String completeMessage = "";
    Cursor& cursor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ModifierMenuComponent)
};
