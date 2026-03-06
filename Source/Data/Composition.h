#pragma once

#include "Data/Sequence.h"
#include "juce_data_structures/juce_data_structures.h"

namespace CompositionIDs
{
#define DECLARE_ID(name) inline const juce::Identifier name { #name };
DECLARE_ID (Composition)
DECLARE_ID (Tempo)
DECLARE_ID (Sequences)
#undef DECLARE_ID

} // namespace CompositionIDs

class Composition : public juce::ValueTree::Listener
{
public:
    Composition();
    explicit Composition (juce::ValueTree existingState);
    ~Composition();

    void createDefaultSequences();

    double getTempo() const;
    void setTempo (double newTemp, juce::UndoManager* undoManager = nullptr);

    juce::ValueTree getSequencesState();
    Sequence& getSequence (size_t index) const;
    const std::vector<std::unique_ptr<Sequence>>& getSequences() const;

    std::vector<MidiNote> extractMidiSequence (size_t seqIndex, double tempo);

    void valueTreeChildAdded (juce::ValueTree& parentTree,
                              juce::ValueTree& childWhichHasBeenAdded);

    void valueTreeChildRemoved (juce::ValueTree& parentTree,
                                juce::ValueTree& childWhichHasBeenRemoved,
                                int indexFromWhichChildWasRemoved);

private:
    juce::ValueTree state;

    int numDefaultSequences { 4 };

    std::vector<std::unique_ptr<Sequence>> sequences;
};
