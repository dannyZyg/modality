#include "Components/Widgets/TextInputWidgetComponent.h"

TextInputWidgetComponent::TextInputWidgetComponent()
{
    setup();
    setWantsKeyboardFocus (true);
}

TextInputWidgetComponent::TextInputWidgetComponent (const juce::String& _title, juce::Value valueToBindTo, int _maxLength)
    : title (_title), maxLength (_maxLength)
{
    setup();

    // Bind the text editor's value object to the provided juce::Value
    // This creates two-way binding - changes to editor update ValueTree and vice versa
    editor.getTextValue().referTo (valueToBindTo);

    // Set initial text from the bound value
    editor.setText (valueToBindTo.toString(), juce::dontSendNotification);

    setWantsKeyboardFocus (true);
}

TextInputWidgetComponent::~TextInputWidgetComponent() {}

void TextInputWidgetComponent::setup()
{
    // Configure for single-line text input
    editor.setMultiLine (false);
    editor.setReturnKeyStartsNewLine (false);

    // Set input restrictions
    if (maxLength > 0)
    {
        editor.setInputRestrictions (maxLength);
    }

    // Add this as a listener to the editor
    editor.addListener (this);

    // Disable right-click popup menu
    editor.setPopupMenuEnabled (false);

    // Make editor visible
    addAndMakeVisible (editor);
}

void TextInputWidgetComponent::paint (juce::Graphics& g)
{
    // Fill background
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // Draw title
    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawText (title, getLocalBounds().removeFromTop (30), juce::Justification::centred, true);

    // Draw border based on state
    if (isSelected())
    {
        float borderThickness = 2.0f;
        juce::Rectangle<int> bounds = getLocalBounds();

        if (isEditMode)
        {
            // Orange border for edit mode
            g.setColour (juce::Colours::orange);
        }
        else
        {
            // Aqua for selected but not editing
            g.setColour (juce::Colours::aqua);
        }

        g.drawRect (bounds.toFloat(), borderThickness);
    }
}

void TextInputWidgetComponent::resized()
{
    auto bounds = getLocalBounds();

    // Remove top 30px for title
    bounds.removeFromTop (30);

    // Add some padding around the editor
    bounds.reduce (5, 2); // 5px horizontal, 2px vertical padding

    // Set editor bounds
    editor.setBounds (bounds);
}

bool TextInputWidgetComponent::keyPressed (const juce::KeyPress& key)
{
    // If selected but not in edit mode, check for edit mode activation keys
    if (isSelected() && !isEditMode)
    {
        // 'i' - enter insert mode at beginning
        if (key == juce::KeyPress::createFromDescription ("i"))
        {
            isEditMode = true;
            editor.grabKeyboardFocus();
            editor.setCaretPosition (0); // Move to beginning
            repaint(); // Update border color
            return true;
        }

        // 'a' - enter insert mode at end (append)
        if (key == juce::KeyPress::createFromDescription ("a"))
        {
            isEditMode = true;
            editor.grabKeyboardFocus();
            editor.moveCaretToEnd (false); // false = don't select text
            repaint(); // Update border color
            return true;
        }

        // Enter - enter insert mode at end
        if (key == juce::KeyPress::returnKey)
        {
            isEditMode = true;
            editor.grabKeyboardFocus();
            editor.moveCaretToEnd (false);
            repaint(); // Update border color
            return true;
        }

        // Other keys handled by parent (j/k navigation, etc.)
        return false;
    }

    // If in edit mode, Escape exits edit mode
    if (isEditMode && key == juce::KeyPress::escapeKey)
    {
        exitEditMode();
        return true;
    }

    // If in edit mode, let editor handle all other keys
    if (isEditMode)
    {
        return false; // Let the editor process typing
    }

    return false;
}

void TextInputWidgetComponent::textEditorTextChanged (juce::TextEditor&)
{
    // Value binding via getTextValue().referTo() handles updates automatically
    // This method can be empty or used for additional validation/feedback
}

void TextInputWidgetComponent::textEditorReturnKeyPressed (juce::TextEditor&)
{
    // Exit edit mode when user presses Enter
    exitEditMode();
}

void TextInputWidgetComponent::textEditorEscapeKeyPressed (juce::TextEditor&)
{
    // Exit edit mode when user presses Escape
    exitEditMode();
}

void TextInputWidgetComponent::textEditorFocusLost (juce::TextEditor&)
{
    // Exit edit mode when editor loses focus
    if (isEditMode)
    {
        exitEditMode();
    }
}

void TextInputWidgetComponent::exitEditMode()
{
    isEditMode = false;
    editor.giveAwayKeyboardFocus();
    grabKeyboardFocus(); // Return focus to the widget itself
    repaint(); // Update border color back to aqua
}
