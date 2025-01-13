#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    // Make sure you set the size of the component after
    // you add any child components.
    setSize (800, 600);
    setFramesPerSecond (60); // This sets the frequency of the update calls.
    setWantsKeyboardFocus(true);
    
    for (int i = 0; i < numPortions; ++i) {
        auto s = std::make_unique<Step>();
        addAndMakeVisible(s.get());
        steps.emplace_back(std::move(s));
    }
}

MainComponent::~MainComponent()
{
}


constexpr const char* MainComponent::ModeToString(Mode m) throw()
{
    switch (m)
    {
        case Mode::normal: return "Normal Mode";
        case Mode::visual: return "Visual Mode";
        default: throw std::invalid_argument("Unimplemented item");
    }
}

//==============================================================================
void MainComponent::update()
{
    // This function is called at the frequency specified by the setFramesPerSecond() call
    // in the constructor. You can use it to update counters, animate values, etc.
    
    for (auto i = 0; i < steps.size(); i++) {
        if (i == selectedPortion) {
            steps[i]->select();
        } else {
            steps[i]->unselect();
        }
    }
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    grabKeyboardFocus();
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::white);
    
    resizeSteps();
    
    g.setFont (20.0f);
    g.setColour(juce::Colours::black);
    
    g.drawText (keyText, 20, getHeight() - 50, 200, 40, juce::Justification::bottomLeft, true);
    g.drawText (ModeToString(mode), getWidth() - 250, getHeight() - 50, 200, 40, juce::Justification::bottomRight, true);
    g.drawLine(0, getHeight()/2, getWidth(), getHeight()/2);
}

void MainComponent::resizeSteps()
{
    int padding = 50;
    int height = getHeight() - 2 * padding;
    int xStart = padding;
    int blockSize = 40;
    
    for (auto i = 0; i < steps.size(); i++)
    {
        int spacing = i * 10;
        steps[i]->setBounds(xStart + i * blockSize + spacing, padding, blockSize, height);
    }
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    
    resizeSteps();
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
        if (selectedPortion <= 0)
            selectedPortion = 0;
        else
            selectedPortion--;

        //if (mode == "v") {
        //    visualSelection.emplace_back(steps[selectedPortion]);
        //}
        return true; // Indicate that the event was handled
    }
    
    if (key.getTextCharacter() == 'l')
    {
        if (selectedPortion >= numPortions -1) selectedPortion = numPortions - 1;
        else
            selectedPortion++;
        
        return true; // Indicate that the event was handled
    }
    
    if (key.getTextCharacter() == 'j')
    {
        steps[selectedPortion]->stepDown();
        return true;
    }
    
    if (key.getTextCharacter() == 'k')
    {
        steps[selectedPortion]->stepUp();
        return true;
    }

    if (key.getTextCharacter() == '^')
    {
        selectedPortion = 0;
        return true;
    }
    
    if (key.getTextCharacter() == '$')
    {
        selectedPortion = numPortions - 1;
        return true;
    }
    
    if (key.getTextCharacter() == 'v')
    {
        mode = Mode::visual;
        return true;
    }
    
    if (key == juce::KeyPress::escapeKey)
    {
        mode = Mode::normal;
        return true;
    }

    return false; // Pass unhandled keys to the base class
}
