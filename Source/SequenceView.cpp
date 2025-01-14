/*
  ==============================================================================

    SequenceView.cpp
    Created: 13 Jan 2025 3:17:50pm
    Author:  Danny Keig

  ==============================================================================
*/

#include <JuceHeader.h>
#include "SequenceView.h"

//==============================================================================
SequenceView::SequenceView(const Sequence& s, const Cursor& c)
    : sequence(s), cursor(c)
{
    for (int i = 0; i < s.steps.size(); ++i) {
        auto step = s.steps[i].get();
        if (step) {
            auto stepView = std::make_unique<StepView>(*step);
            addAndMakeVisible(stepView.get());
            stepViews.emplace_back(std::move(stepView));
        }
    }
}

SequenceView::~SequenceView()
{
}

void SequenceView::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (juce::Colours::white);
}

void SequenceView::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

void SequenceView::resizeSteps()
{
    int padding = 50;
    int height = getHeight() - 2 * padding;
    int xStart = padding;
    int blockSize = 40;
    
    for (auto i = 0; i < stepViews.size(); i++)
    {
        int spacing = i * 10;
        stepViews[i]->setBounds(xStart + i * blockSize + spacing, padding, blockSize, height);
    }
}

//==============================================================================
void SequenceView::update()
{
    // This function is called at the frequency specified by the setFramesPerSecond() call
    // in the constructor. You can use it to update counters, animate values, etc.
    
    for (int i = 0; i < stepViews.size(); i++) {
        
        stepViews[i]->setPos(visibleRange);
        
    }
}
