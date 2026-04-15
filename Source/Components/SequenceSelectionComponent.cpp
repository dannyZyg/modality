#include "SequenceSelectionComponent.h"
#include "juce_graphics/juce_graphics.h"
#include <JuceHeader.h>

//==============================================================================
SequenceSelectionComponent::SequenceSelectionComponent (const Cursor& curs, Composition& comp)
    : cursor (curs), composition (comp)
{
    // Bind to each sequence's name Value for automatic updates
    const auto& sequences = composition.getSequences();
    for (const auto& seqPtr : sequences)
    {
        if (seqPtr)
        {
            auto nameValue = seqPtr->getNameAsValue();
            nameValue.addListener (this);
            sequenceNameValues.push_back (nameValue);
        }
    }
}

SequenceSelectionComponent::~SequenceSelectionComponent()
{
    for (auto& value : sequenceNameValues)
    {
        value.removeListener (this);
    }
}

void SequenceSelectionComponent::paint (juce::Graphics& g)
{
    float width = static_cast<float> (getWidth());
    float height = static_cast<float> (getHeight());

    const auto& sequences = composition.getSequences();
    const Sequence& selectedSequence = cursor.getSelectedSequence();

    float padding = 20.0f;

    auto numSeqs = static_cast<float> (sequences.size());

    auto barWidth = (width - ((numSeqs - 1.0f) * padding)) / numSeqs;

    for (size_t i = 0; i < sequences.size(); ++i)
    {
        const auto& seqPtr = sequences[i];
        auto fi = static_cast<float> (i);
        auto rect = juce::Rectangle<float> (
            (fi * barWidth) + (fi * padding),
            0,
            barWidth,
            height);

        g.setColour (juce::Colours::grey);
        g.drawRect (rect);

        if (seqPtr && seqPtr.get() == &selectedSequence)
        {
            g.setColour (juce::Colours::orchid);
            g.fillRect (rect);
        }

        if (seqPtr)
        {
            juce::String name = seqPtr->getName();

            if (name.isEmpty())
            {
                name = "Sequence " + juce::String (i + 1);
            }

            if (seqPtr.get() == &selectedSequence)
            {
                g.setColour (juce::Colours::white);
            }
            else
            {
                g.setColour (juce::Colours::black);
            }

            g.setFont (12.0f);
            bool useEllipsis = true;
            g.drawText (name, rect, juce::Justification::centred, useEllipsis);
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

void SequenceSelectionComponent::valueChanged ([[maybe_unused]] juce::Value& value)
{
    repaint();
}
