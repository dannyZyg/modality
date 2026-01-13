#include "Components/ContextualMenuComponent.h"
#include "juce_graphics/juce_graphics.h"

//==============================================================================
ContextualMenuComponent::ContextualMenuComponent()
    : messageTimeoutMs (0)
{
    setWantsKeyboardFocus (true);
    startTimer (100); // Start timer for UI updates and message timeout
}

ContextualMenuComponent::~ContextualMenuComponent()
{
    stopTimer();
}

void ContextualMenuComponent::focusGained (FocusChangeType cause)
{
    // Refresh UI when focus is gained
    repaint();
}

bool ContextualMenuComponent::keyPressed (const juce::KeyPress& key)
{
    // Always handle Escape for navigation back/exit
    if (key == juce::KeyPress::escapeKey)
    {
        navigateBack();
        return true; // Key was handled
    }

    if (currentMenuNode)
    {
        for (const auto& child : currentMenuNode->children)
        {
            if (child->navShortcut == key)
            {
                navigateTo (child.get());
            }
        }
    }

    // Don't let key presses bubble up to main component
    return true;
}

void ContextualMenuComponent::displayMenu (MenuNode* rootNode)
{
    if (! rootNode)
        return;

    // Clear menu history
    static_cast<void> (backStack.empty());

    currentMenuNode = rootNode;
    setVisible (true);
    repaint();
    showMessage ("Menu displayed. Press Escape to exit.", 2000);

    // This needs to be called after the component has a valid size and is visible, otherwise focus won't be gained!
    grabKeyboardFocus();
}

void ContextualMenuComponent::paint (juce::Graphics& g)
{
    if (! currentMenuNode)
    {
        return;
    }

    g.fillAll (juce::Colours::darkgrey.withAlpha (0.9f));

    drawContextualContent (g);

    drawShortcutHelp (g);

    drawMessage (g);

    if (currentMenuNode->children.size() > 0)
    {
        drawSubNavOptions (g);
    }
}

void ContextualMenuComponent::drawContextualContent (juce::Graphics& g)
{
    g.setColour (juce::Colours::white);
    g.setFont (16.0f);
    g.drawText (currentMenuNode->title, getLocalBounds().reduced (10), juce::Justification::centredTop);
}

void ContextualMenuComponent::drawShortcutHelp (juce::Graphics& g) {}

void ContextualMenuComponent::drawMessage (juce::Graphics& g)
{
    if (currentMessage.isNotEmpty())
    {
        // Draw message in a visible box
        auto bounds = getLocalBounds().reduced (20);
        auto msgBounds = bounds.removeFromBottom (40).reduced (10);

        g.setColour (juce::Colours::black.withAlpha (0.7f));
        g.fillRoundedRectangle (msgBounds.toFloat(), 5.0f);

        g.setColour (juce::Colours::white);
        g.setFont (16.0f);
        g.drawText (currentMessage, msgBounds, juce::Justification::centred);
    }
}

void ContextualMenuComponent::resized()
{
    if (currentMenuNode && currentMenuNode->component)
    {
        auto titleHeight = 40;
        auto messageHeight = 60;
        auto verticalPadding = 10;
        auto horizontalPadding = 20;

        auto y = titleHeight + verticalPadding;
        auto height = getHeight() - titleHeight - messageHeight - (2 * verticalPadding);
        auto width = getWidth() - (2 * horizontalPadding);
        auto bounds = juce::Rectangle<int> (horizontalPadding, y, width, height);

        currentMenuNode->component->setBounds (bounds);
    }
}

void ContextualMenuComponent::timerCallback()
{
    // Handle message timeout
    if (currentMessage.isNotEmpty() && messageTimeoutMs > 0)
    {
        messageTimeoutMs -= 100; // Assuming 100ms timer interval

        if (messageTimeoutMs <= 0)
        {
            currentMessage = "";
            repaint();
        }
    }
}

void ContextualMenuComponent::showMessage (juce::String message, int timeoutMs)
{
    currentMessage = message;
    messageTimeoutMs = timeoutMs;
    repaint();
}

void ContextualMenuComponent::navigateTo (MenuNode* childNode)
{
    if (! childNode || ! currentMenuNode)
        return;

    backStack.push (currentMenuNode); // Push current node onto the stack
    currentMenuNode = childNode; // Move to the child node

    if (currentMenuNode->children.size() == 0 && currentMenuNode->component)
    {
        addAndMakeVisible (currentMenuNode->component.get());
        currentMenuNode->component->repaint();
        currentMenuNode->component->grabKeyboardFocus();
    }

    // Update display
    resized();
    repaint();
}

void ContextualMenuComponent::navigateBack()
{
    if (! backStack.empty())
    {
        // Remove existing component
        if (currentMenuNode->component)
            removeChildComponent (currentMenuNode->component.get());

        currentMenuNode = backStack.top(); // Go back to the previous node
        backStack.pop(); // Remove from stack

        // Add new component
        if (currentMenuNode->component)
        {
            addAndMakeVisible (currentMenuNode->component.get());
            currentMenuNode->component->grabKeyboardFocus();
        }

        resized();
        repaint();
    }
    else
    {
        closeMenu(); // No more nodes in the stack, exit the menu
    }
}

void ContextualMenuComponent::closeMenu()
{
    currentMenuNode = nullptr; // Clear current node
    while (! backStack.empty()) // Clear the stack
        backStack.pop();

    setVisible (false);
    repaint();
}

void ContextualMenuComponent::drawSubNavOptions (juce::Graphics& g)
{
    if (currentMenuNode == nullptr)
    {
        return;
    }

    g.setColour (juce::Colours::lightgrey);
    g.setFont (juce::Font (20.0f));

    int yOffset = 50;
    int itemHeight = 30;

    for (auto& c : currentMenuNode->children)
    {
        juce::String keyText = c->navShortcut.getTextDescription().toLowerCase();
        juce::String itemLabel = c->title;
        juce::String displayText = keyText + " -> " + itemLabel;

        g.drawText (displayText,
                    getLocalBounds().getX() + 50,
                    yOffset,
                    getLocalBounds().getWidth() - 100,
                    itemHeight,
                    juce::Justification::left,
                    true);

        yOffset += itemHeight;
    }

    juce::String prevTitle = backStack.empty() ? "Close Menu" : backStack.top()->title;

    g.setColour (juce::Colours::lightgrey);
    g.setFont (juce::Font (16.0f, juce::Font::italic));
    g.drawText ("Press ESC: " + prevTitle,
                getLocalBounds().getX() + 50,
                getLocalBounds().getBottom() - 30, // Closer to the absolute bottom
                getLocalBounds().getWidth() - 100,
                20,
                juce::Justification::left,
                true);
}
