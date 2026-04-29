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

std::vector<MidiNote> Composition::extractMidiSequenceForBeatRange (size_t seqIndex, double startBeat, double endBeat, double tempo)
{
    std::vector<MidiNote> midiClip;

    auto& seq = getSequence (seqIndex);
    double loopLengthBeats = seq.getLengthBeats();
    
    // Clear any stale triggered state from previous scheduling passes
    for (auto& n : seq.notes)
    {
        n->clearLastTriggeredMidiNote();
    }

    // Convert global beats to loop-local beats
    double localStartBeat = std::fmod (startBeat, loopLengthBeats);
    double localEndBeat = localStartBeat + (endBeat - startBeat);

    // Handle wraparound case where we need notes from multiple loop iterations
    bool wrapsAround = localEndBeat > loopLengthBeats;

    for (auto& n : seq.notes)
    {
        auto midi = n->asMidiNote (seq.getTimeline(), seq.getScale(), tempo, seq.getRootNote());

        if (! midi)
            continue;

        double noteStartBeat = midi->startTime / (60.0 / tempo); // Convert seconds back to beats

        // Check if note falls within the requested range
        bool inRange = false;
        double adjustedStartTime = 0.0;

        if (wrapsAround)
        {
            // Need to check both before and after wraparound
            if (noteStartBeat >= localStartBeat && noteStartBeat < loopLengthBeats)
            {
                // Note is in the first part (before wrap)
                inRange = true;
                adjustedStartTime = noteStartBeat - localStartBeat;
            }
            else if (noteStartBeat < (localEndBeat - loopLengthBeats))
            {
                // Note is in the wrapped part (after wrap)
                inRange = true;
                adjustedStartTime = (loopLengthBeats - localStartBeat) + noteStartBeat;
            }
        }
        else
        {
            // No wraparound - simple range check
            if (noteStartBeat >= localStartBeat && noteStartBeat < localEndBeat)
            {
                inRange = true;
                adjustedStartTime = noteStartBeat - localStartBeat;
            }
        }

        if (inRange)
        {
            // Convert adjusted beat time back to seconds
            double adjustedStartTimeSeconds = adjustedStartTime * (60.0 / tempo);

            // Store the post-modifier result on the note for the UI to read for visualisation
            // Use loop-local start time in seconds so it's directly comparable to
            // the looped playhead position in SequenceComponent::paint
            MidiNote triggered = *midi;
            triggered.startTime = noteStartBeat * (60.0 / tempo);
            n->setLastTriggeredMidiNote (triggered);

            // Do not schedule muted notes for playback, but still allow the UI to show them
            if (! midi->isMuted)
                midiClip.emplace_back (adjustedStartTimeSeconds, midi->noteNumber, midi->velocity, midi->duration);
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

bool Composition::isDirty() const { return dirty; }

void Composition::reset()
{
    state.removeAllChildren (nullptr);
    sequences.clear();
    currentFile = juce::File {};
    createDefaultSequences();
    setIsDirty (false);
}

void Composition::setIsDirty (bool v)
{
    if (isDirty() != v)
    {
        dirty = v;
        sendChangeMessage();
    }
}
