#include "Data/Composition.h"
#include "Data/Sequence.h"
#include "juce_core/juce_core.h"
#include "juce_core/system/juce_PlatformDefs.h"
#include "juce_data_structures/juce_data_structures.h"

Composition::Composition() : state (CompositionIDs::Composition)
{
    state.addListener (this);
    createDefaultSequences();
}

Composition::Composition (juce::ValueTree existingState)
    : state (std::move (existingState))
{
    jassert (existingState.hasType (CompositionIDs::Composition));
    state.addListener (this);
}

Composition::~Composition()
{
    state.removeListener (this);
}

void Composition::createDefaultSequences()
{
    auto sequencesState = getSequencesState();

    if (! sequencesState.isValid())
    {
        sequencesState = juce::ValueTree (CompositionIDs::Sequences);
        state.addChild (sequencesState, -1, nullptr);
    }

    for (int i = 0; i < numDefaultSequences; ++i)
    {
        juce::ValueTree seqState (SequenceIDs::Sequence);
        sequencesState.addChild (seqState, -1, nullptr);
    }
}

juce::ValueTree Composition::getSequencesState() { return state.getChildWithName (CompositionIDs::Sequences); }

double Composition::getTempo() const
{
    return static_cast<double> (state.getProperty (CompositionIDs::Tempo));
}

void Composition::setTempo (double newTemp, juce::UndoManager* undoManager)
{
    state.setProperty (CompositionIDs::Tempo, newTemp, undoManager);
}

void Composition::valueTreeChildAdded (juce::ValueTree& parentTree,
                                       juce::ValueTree& childWhichHasBeenAdded)
{
    if (parentTree.hasType (CompositionIDs::Sequences))
    {
        auto sequence = std::make_unique<Sequence> (childWhichHasBeenAdded);
        sequences.emplace_back (std::move (sequence));
    }
    setIsDirty (true);
}

void Composition::valueTreeChildRemoved (juce::ValueTree& parentTree,
                                         juce::ValueTree& childWhichHasBeenRemoved,
                                         [[maybe_unused]] int indexFromWhichChildWasRemoved)
{
    if (parentTree.hasType (CompositionIDs::Sequences))
    {
        std::erase_if (sequences, [&] (const auto& sequence)
                       { return sequence->getState() == childWhichHasBeenRemoved; });
    }
    setIsDirty (true);
}

void Composition::valueTreePropertyChanged (juce::ValueTree& treeWhosePropertyHasChanged,
                                            const juce::Identifier& property)
{
    setIsDirty (true);
}

void Composition::valueTreeChildOrderChanged (ValueTree& treeWhichChildrenBelongTo,
                                              int oldChildIndex,
                                              int newChildIndex)
{
    setIsDirty (true);
}

Sequence& Composition::getSequence (size_t index) const
{
    if (index >= sequences.size())
    {
        std::string m = "Seq index out of bounds: " + std::to_string (index);
        juce::Logger::writeToLog (m);
        throw std::out_of_range (m);
    }
    return *sequences[index];
}

const std::vector<std::unique_ptr<Sequence>>& Composition::getSequences() const
{
    return sequences;
}

std::vector<MidiNote> Composition::extractMidiSequence (size_t seqIndex, double tempo)
{
    std::vector<MidiNote> midiClip;

    auto& seq = getSequence (seqIndex);

    for (auto& n : seq.notes)
    {
        auto midi = n->asMidiNote (seq.getTimeline(), seq.getScale(), tempo);

        if (midi)
        {
            midiClip.emplace_back (*midi);
        }
    }
    return midiClip;
}

juce::ValueTree Composition::getState()
{
    return state;
}

bool Composition::hasFile() { return currentFile.existsAsFile(); }

bool Composition::save()
{
    if (hasFile())
    {
        return saveToFile (currentFile);
    }
    return false;
}

bool Composition::saveAs (juce::File& f)
{
    return saveToFile (f);
}

bool Composition::saveToFile (juce::File& f)
{
    auto stateXml = getState().toXmlString();

    f.replaceWithText (stateXml);

    currentFile = f;
    setIsDirty (false);
    return true;
}

bool Composition::loadFromFile (juce::File& f)
{
    if (! f.existsAsFile())
    {
        juce::AlertWindow::showMessageBoxAsync (
            juce::MessageBoxIconType::WarningIcon,
            "File Not Found: " + f.getFullPathName(),
            "The file could not be found. Please check the path and try again.",
            "OK",
            nullptr);

        return false;
    }

    auto xml = juce::XmlDocument::parse (f);

    if (xml == nullptr)
    {
        juce::AlertWindow::showMessageBoxAsync (
            juce::MessageBoxIconType::WarningIcon,
            "File Corrupt: " + f.getFullPathName(),
            "The file could not be loaded.",
            "OK",
            nullptr);

        return false;
    }

    auto loadedTree = juce::ValueTree::fromXml (*xml);

    if (! loadedTree.isValid())
    {
        juce::AlertWindow::showMessageBoxAsync (
            juce::MessageBoxIconType::WarningIcon,
            "File not supported: " + f.getFullPathName(),
            "The file could not be loaded.",
            "OK",
            nullptr);

        return false;
    }

    if (loadedTree.getType() != juce::Identifier ("Composition"))
    {
        juce::AlertWindow::showMessageBoxAsync (
            juce::MessageBoxIconType::WarningIcon,
            "File not supported: " + f.getFullPathName(),
            "The file could not be loaded.",
            "OK",
            nullptr);
        return false;
    }

    state.removeAllChildren (nullptr);
    state.copyPropertiesAndChildrenFrom (loadedTree, nullptr);

    sequences.clear();
    for (auto seqState : state.getChildWithName (CompositionIDs::Sequences))
    {
        sequences.push_back (std::make_unique<Sequence> (seqState));
    }

    return true;
}

bool Composition::isDirty() { return dirty; }

void Composition::setIsDirty (bool v)
{
    DBG ("SETTING IS DIRTY: ");
    if (isDirty() != v)
    {
        dirty = v;
        DBG ("sending change");
        sendChangeMessage();
    }
}
