#include "SequenceSelectionComponent.h"
#include "juce_core/system/juce_PlatformDefs.h"
#include "juce_graphics/juce_graphics.h"
#include <JuceHeader.h>

//==============================================================================
SequenceSelectionComponent::SequenceSelectionComponent (const Cursor& c)
    : cursor (c)
{
}

SequenceSelectionComponent::~SequenceSelectionComponent() {}

void SequenceSelectionComponent::paint (juce::Graphics& g)
{
    float width = static_cast<float> (getWidth());
    float height = static_cast<float> (getHeight());
    // auto bounds = getLocalBounds();
    // g.setColour (juce::Colours::blue);
    // g.fillRect (bounds);

    const auto& sequences = cursor.getSequences();
    const Sequence& selectedSequence = cursor.getSelectedSequence();

    int padding = 20;

    size_t numSeqs = sequences.size();

    auto barWidth = (width - ((numSeqs - 1) * padding)) / numSeqs;

    for (size_t i = 0; i < sequences.size(); ++i)
    {
        const auto& seqPtr = sequences[i]; // Access the unique_ptr at the current index
        //
        auto rect = juce::Rectangle<float> (
            (i * barWidth) + (i * padding),
            0,
            barWidth,
            height);

        g.setColour (juce::Colours::grey);
        g.drawRect (rect);

        if (seqPtr && seqPtr.get() == &selectedSequence)
        {
            g.setColour (juce::Colours::orchid);
            g.fillRect (rect);
            // seqPtr points to the currently selected sequence
            // You can perform actions specific to the selected sequence here
            // ... your code ...
        }
        else if (seqPtr)
        {
            // seqPtr points to a non-selected sequence
            // ... other actions ...
        }
    }
}

void SequenceSelectionComponent::resized()
{
}

//==============================================================================
void SequenceSelectionComponent::update()
{
}
