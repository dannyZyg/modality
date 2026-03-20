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

class Composition : public juce::ValueTree::Listener, public juce::ChangeBroadcaster
{
public:
    Composition();
    explicit Composition (juce::ValueTree existingState);
    ~Composition() override;

    void createDefaultSequences();

    double getTempo() const;
    void setTempo (double newTemp, juce::UndoManager* undoManager = nullptr);

    juce::ValueTree getSequencesState();
    Sequence& getSequence (size_t index) const;
    const std::vector<std::unique_ptr<Sequence>>& getSequences() const;

    std::vector<MidiNote> extractMidiSequence (size_t seqIndex, double tempo);

    void valueTreeChildAdded (juce::ValueTree& parentTree,
                              juce::ValueTree& childWhichHasBeenAdded) override;

    void valueTreeChildRemoved (juce::ValueTree& parentTree,
                                juce::ValueTree& childWhichHasBeenRemoved,
                                int indexFromWhichChildWasRemoved) override;

    void valueTreePropertyChanged (juce::ValueTree& treeWhosePropertyHasChanged,
                                   const juce::Identifier& property) override;

    void valueTreeChildOrderChanged (ValueTree& treeWhichChildrenBelongTo,
                                     int oldChildIndex,
                                     int newChildIndex) override;

    juce::ValueTree getState();

    bool save();
    bool saveAs (juce::File& f);
    bool saveToFile (juce::File& f);
    bool hasFile();

    bool loadFromFile (juce::File& f);

    bool isDirty();
    void setIsDirty (bool v);

private:
    juce::ValueTree state;
    juce::File currentFile;
    bool dirty { false };

    int numDefaultSequences { 4 };

    std::vector<std::unique_ptr<Sequence>> sequences;
};
