#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent() : sequenceComponent(sequence, cursor)
{
    // Make sure you set the size of the component after
    // you add any child components.
    setSize (800, 600);
    setFramesPerSecond (60); // This sets the frequency of the update calls.
    setWantsKeyboardFocus(true);
    addAndMakeVisible(sequenceComponent);
    
    cursor.selectSequence(&sequence);
}

MainComponent::~MainComponent()
{
}

//==============================================================================
void MainComponent::update()
{
    // This function is called at the frequency specified by the setFramesPerSecond() call
    // in the constructor. You can use it to update counters, animate values, etc.
    sequenceComponent.update();
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    grabKeyboardFocus();
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::white);
    
    g.setFont (20.0f);
    g.setColour(juce::Colours::black);
    
    g.drawText (keyText, 20, getHeight() - 50, 200, 40, juce::Justification::bottomLeft, true);
    g.drawText (cursor.getModeName(), getWidth() - 250, getHeight() - 50, 200, 40, juce::Justification::bottomRight, true);
    g.drawLine(0, getHeight()/2, getWidth(), getHeight()/2);
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    sequenceComponent.setBounds(50, 50, getWidth() - 100, getHeight() - 100);
    sequenceComponent.resizeSteps();
}

bool MainComponent::keyPressed (const juce::KeyPress& key)
{
    keyText = "Key: " + key.getTextDescription();
    
    // Handle key presses here
    if (key == juce::KeyPress::spaceKey)
    {
        juce::Logger::writeToLog("Space key pressed!");
        return true; // Indicate that the event was handled
    }
    
    if (key.getTextCharacter() == 'h')
    {
        cursor.moveLeft();
        return true; // Indicate that the event was handled
    }
    
    if (key.getTextCharacter() == 'l')
    {
        cursor.moveRight();
        return true; // Indicate that the event was handled
    }
    
    if (key.getTextCharacter() == 'j')
    {
        cursor.moveDown();
        return true;
    }
    
    if (key.getTextCharacter() == 'k')
    {
        cursor.moveUp();
        return true;
    }

    if (key.getTextCharacter() == '^')
    {
        cursor.jumpToStart();
        return true;
    }
    
    if (key.getTextCharacter() == '$')
    {
        cursor.jumpToEnd();
        return true;
    }
    
    if (key.getTextCharacter() == 'v')
    {
        cursor.enableVisualMode();
        return true;
    }
    
    if (key.getTextCharacter() == 'm')
    {
        cursor.enableVisualMode();
        return true;
    }

    if (key == juce::KeyPress::escapeKey)
    {
        cursor.enableNormalMode();
        return true;
    }
    
    if (key.getTextCharacter() == 'a')
    {
        cursor.addNote();
        return true;
    }
    
    if (key.getTextCharacter() == 'r')
    {
        cursor.removeNote();
        return true;
    }

    return false; // Pass unhandled keys to the base class
}
