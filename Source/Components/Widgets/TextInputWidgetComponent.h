#pragma once
#include "Components/Widgets/ISelectableWidget.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include <JuceHeader.h>

class TextInputWidgetComponent : public ISelectableWidget, public juce::TextEditor::Listener
{
public:
    //==============================================================================
    TextInputWidgetComponent();

    // Value-bound constructor - binds text editor to a ValueTree property
    TextInputWidgetComponent (const juce::String& _title, juce::Value valueToBindTo, int _maxLength = 64);

    virtual ~TextInputWidgetComponent() override;

    void setup();

    bool keyPressed (const juce::KeyPress& key) override;
    void paint (juce::Graphics&) override;
    void resized() override;

    // TextEditor::Listener interface
    void textEditorTextChanged (juce::TextEditor&) override;
    void textEditorReturnKeyPressed (juce::TextEditor&) override;
    void textEditorEscapeKeyPressed (juce::TextEditor&) override;
    void textEditorFocusLost (juce::TextEditor&) override;

private:
    void exitEditMode();

    juce::TextEditor editor;
    juce::String title = "text editor";
    int maxLength = 64;
    bool isEditMode = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TextInputWidgetComponent)
};
