#include "SequenceSelectionComponent.h"
#include "AppColours.h"
#include "Data/Sequence.h"
#include "juce_graphics/juce_graphics.h"
#include <JuceHeader.h>

//==============================================================================
SequenceSelectionComponent::SequenceSelectionComponent (const Cursor& curs, Composition& comp)
    : cursor (curs), composition (comp)
{
    // Bind to each sequence's name, muted, and soloed Values for automatic updates
    const auto& sequences = composition.getSequences();
    for (const auto& seqPtr : sequences)
    {
        if (seqPtr)
        {
            auto nameValue = seqPtr->getNameAsValue();
            nameValue.addListener (this);
            sequenceNameValues.push_back (nameValue);

            auto mutedValue = seqPtr->getState().getPropertyAsValue (SequenceIDs::Muted, nullptr);
            mutedValue.addListener (this);
            sequenceMutedValues.push_back (mutedValue);

            auto soloedValue = seqPtr->getState().getPropertyAsValue (SequenceIDs::Soloed, nullptr);
            soloedValue.addListener (this);
            sequenceSoloedValues.push_back (soloedValue);
        }
    }

    startTimer (500); // Flash timer for soloed sequences
}

SequenceSelectionComponent::~SequenceSelectionComponent()
{
    stopTimer();

    for (auto& value : sequenceNameValues)
        value.removeListener (this);
    for (auto& value : sequenceMutedValues)
        value.removeListener (this);
    for (auto& value : sequenceSoloedValues)
        value.removeListener (this);
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

        bool isSelected = seqPtr && seqPtr.get() == &selectedSequence;
        bool isMuted = seqPtr && seqPtr->isMuted();
        bool isSoloed = seqPtr && seqPtr->isSoloed();

        // Determine fill colour
        if (isSelected)
        {
            if (isSoloed)
                g.setColour (AppColours::sequenceSoloFill);
            else if (isMuted)
                g.setColour (AppColours::sequenceMutedSelected);
            else
                g.setColour (AppColours::sequenceSelected);

            g.fillRect (rect);
        }
        else if (isMuted)
        {
            // Greyed-out fill for muted unselected sequences
            g.setColour (AppColours::sequenceMutedFill);
            g.fillRect (rect);
        }
        else if (isSoloed)
        {
            // Gold fill for soloed unselected sequences
            g.setColour (AppColours::sequenceSoloFill.withAlpha (0.5f));
            g.fillRect (rect);
        }

        // Outline
        if (isSoloed && flashState)
        {
            // Flashing bright outline for soloed sequences
            g.setColour (AppColours::sequenceSoloFlash);
            g.drawRect (rect, 2.0f);
        }
        else if (isMuted)
        {
            g.setColour (AppColours::sequenceMutedOutline);
            g.drawRect (rect);
        }
        else
        {
            g.setColour (AppColours::sequenceOutline);
            g.drawRect (rect);
        }

        if (seqPtr)
        {
            juce::String name = seqPtr->getName();

            if (name.isEmpty())
                name = "Sequence " + juce::String (i + 1);

            if (isMuted)
                g.setColour (AppColours::sequenceTextMuted);
            else if (isSelected)
                g.setColour (AppColours::sequenceTextSelected);
            else
                g.setColour (AppColours::sequenceText);

            g.setFont (12.0f);
            bool useEllipsis = true;
            g.drawText (name, rect, juce::Justification::centred, useEllipsis);
        }
    }
}

void SequenceSelectionComponent::timerCallback()
{
    // Check if any sequence is soloed — only repaint when needed
    const auto& sequences = composition.getSequences();
    bool anySoloed = std::any_of (sequences.begin(), sequences.end(), [] (const auto& s)
                                  { return s && s->isSoloed(); });
    if (anySoloed)
    {
        flashState = ! flashState;
        repaint();
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
