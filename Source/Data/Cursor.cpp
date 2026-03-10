/*
  ==============================================================================

    Cursor.cpp
    Created: 13 Jan 2025 3:09:08pm
    Author:  Danny Keig

  ==============================================================================
*/

#include "Cursor.h"
#include "Data/Note.h"
#include "Data/Scale.h"
#include "Data/Selection.h"
#include "juce_core/system/juce_PlatformDefs.h"
#include "juce_data_structures/juce_data_structures.h"
#include <ios>

Cursor::Cursor (Composition& comp) : composition (comp), randomGenerator (std::random_device()())
{
    selectSequence (0);
}

Cursor::~Cursor() {}

void Cursor::createSequence()
{
    //TODO
}

void Cursor::selectSequence (size_t index)
{
    selectedSeqIndex = index;
    selectNote (0);
}

void Cursor::undo()
{
    undoManager.undo();
}

void Cursor::redo()
{
    undoManager.redo();
}

void Cursor::selectNote (size_t nIndex)
{
    selectedNoteIndex = nIndex;
}

bool Cursor::isSequenceSelected (const Sequence& otherSequence) const
{
    Sequence& selectedSequence = getSelectedSequence();
    return &selectedSequence == &otherSequence;
}

void Cursor::moveCursorSelection (Direction d)
{
    moveNotesInSelection (d);
    move (d, Selection::MoveMode::shift);
}

void Cursor::moveNotesInSelection (Direction d)
{
    auto& seq = getSelectedSequence();
    std::vector<Note*> notesToMove;

    // Collect notes first, then move. Otherwise we are continually adding the notes we have just moved.
    for (const Position& pos : getVisualSelectionPositions())
    {
        for (auto& note : seq.notes)
        {
            if (Division::isEqual (pos.xTimepoint.value, note->getStartTime())
                && Division::isEqual (pos.yDegree.value, note->getDegree()))
            {
                notesToMove.push_back (note.get());
            }
        }
    }

    for (auto* note : notesToMove)
    {
        switch (d)
        {
            case Direction::left:
            {
                //TODO needs bounds checking
                double newStartTime = note->getStartTime() - seq.getTimeline().getStepSize();
                note->setStartTime (newStartTime, &undoManager);
                break;
            }
            case Direction::right:
            {
                //TODO needs bounds checking
                double newStartTime = note->getStartTime() + seq.getTimeline().getStepSize();
                note->setStartTime (newStartTime, &undoManager);
                break;
            }
            case Direction::up:
            {
                //TODO needs bounds checking?
                Degree currentDeg = note->getDegree();
                Degree higherDeg = seq.getScale().getHigher (currentDeg);
                note->setDegree (higherDeg.value, &undoManager);
                break;
            }
            case Direction::down:
            {
                //TODO needs bounds checking?
                Degree currentDeg = note->getDegree();
                Degree lowerDeg = seq.getScale().getLower (currentDeg);
                note->setDegree (lowerDeg.value, &undoManager);
                break;
            }
            default:
                break;
        }
    }
}

// Moves the cursor and relevant cursor selection
// If in a visual mode, new notes will be added to the selection if they fall within the boundary
// TODO: Also remove notes from selection?
void Cursor::move (Direction d, Selection::MoveMode moveMode)
{
    switch (d)
    {
        case Direction::left:
            cursorPosition.xTimepoint = getSelectedSequence().getPrevTimelineStep (cursorPosition.xTimepoint);
            break;
        case Direction::right:
            cursorPosition.xTimepoint = getSelectedSequence().getNextTimelineStep (cursorPosition.xTimepoint);
            break;
        case Direction::up:
            cursorPosition.yDegree = getCurrentScale().getHigher (cursorPosition.yDegree);
            break;
        case Direction::down:
            cursorPosition.yDegree = getCurrentScale().getLower (cursorPosition.yDegree);
            break;
        default:
            break;
    }

    if (moveMode == Selection::MoveMode::shift)
    {
        visualSelection.moveSelection (getCurrentTimeline(), getCurrentScale(), d);
    }
    else if (moveMode == Selection::MoveMode::extend)
    {
        if (isVisualLineMode())
        {
            visualSelection.addToVisualLineSelection (cursorPosition, getCurrentTimeline(), getCurrentScale());
        }

        if (isVisualBlockMode())
        {
            visualSelection.addToVisualBlockSelection (cursorPosition, getCurrentTimeline(), getCurrentScale());
        }
    }
}

void Cursor::jumpToStart()
{
    cursorPosition.xTimepoint.value = getCurrentTimeline().getLowerBound();
}

void Cursor::jumpToEnd()
{
    cursorPosition.xTimepoint.value = getCurrentTimeline().getUpperBound() - getCurrentTimeline().getStepSize();
}

void Cursor::jumpForwardBeat()
{
    cursorPosition.xTimepoint.value = getSelectedSequence().getNextTimelineStep (cursorPosition.xTimepoint.value, Division::whole).value;
}

void Cursor::jumpBackBeat()
{
    cursorPosition.xTimepoint.value = getSelectedSequence().getPrevTimelineStep (cursorPosition.xTimepoint.value, Division::whole).value;
}

void Cursor::enableNormalMode()
{
    if (isVisualLineMode() || isVisualBlockMode())
    {
        visualSelection.clear();
    }

    mode = Mode::normal;
}

void Cursor::enableVisualLineMode()
{
    mode = Mode::visualLine;
    visualSelection.addToVisualLineSelection (cursorPosition, getCurrentTimeline(), getCurrentScale());
}

void Cursor::enableVisualBlockMode()
{
    mode = Mode::visualBlock;
    visualSelection.addToVisualBlockSelection (cursorPosition, getCurrentTimeline(), getCurrentScale());
}

void Cursor::enableInsertMode()
{
    mode = Mode::insert;
}

const Mode Cursor::getMode() const
{
    return mode;
}

const juce::String Cursor::getModeName() const
{
    return modeToString (mode);
}

constexpr const char* Cursor::modeToString (Mode m) const
{
    switch (m)
    {
        case Mode::normal:
            return "NORMAL";
        case Mode::visualBlock:
            return "V-BLOCK";
        case Mode::visualLine:
            return "V-LINE";
        case Mode::insert:
            return "INSERT";
        case Mode::noteEdit:
            return "NOTE-EDIT";
        default:
            return "UNKOWN";
    }
}

size_t Cursor::getStepIndex()
{
    return selectedStepIndex;
}

size_t Cursor::getNoteIndex()
{
    return selectedNoteIndex;
}

void Cursor::previewNote()
{
}

void Cursor::previewStep()
{
}

Sequence& Cursor::getSequence (size_t index) const
{
    return composition.getSequence (index);
}

Sequence& Cursor::getSelectedSequence() const
{
    return getSequence (selectedSeqIndex);
}

void Cursor::insertNote()
{
    juce::ValueTree noteState (NoteIDs::Note);

    noteState.setProperty (NoteIDs::Degree, cursorPosition.yDegree.value, nullptr);
    noteState.setProperty (NoteIDs::StartTime, cursorPosition.xTimepoint.value, nullptr);
    noteState.setProperty (NoteIDs::Duration, getCurrentTimeline().getStepSize(), nullptr);
    noteState.setProperty (NoteIDs::Velocity, 100, nullptr);

    getSelectedSequence().insertNote (noteState, &undoManager);
    DBG ("Inserted" + noteState.toXmlString());
}

void Cursor::removeNotesAtCursor()
{
    if (isNormalMode() || isInsertMode())
    {
        double degMin = cursorPosition.yDegree.value;
        double degMax = degMin;
        double timeStart = cursorPosition.xTimepoint.value;
        double timeEnd = timeStart + getCurrentTimeline().getStepSize();

        getSelectedSequence().removeNotes (timeStart, timeEnd, degMin, degMax, &undoManager);
    }
    else if (isVisualLineMode() || isVisualBlockMode())
    {
        double degMin = visualSelection.getLowestPosition().yDegree.value;
        double degMax = visualSelection.getHighestPosition().yDegree.value;
        double timeStart = visualSelection.getEarliestPosition().xTimepoint.value;
        double timeEnd = visualSelection.getLatestPosition().xTimepoint.value + getCurrentTimeline().getStepSize();

        getSelectedSequence().removeNotes (timeStart, timeEnd, degMin, degMax, &undoManager);
    }
}

const juce::String Cursor::readableCursorPosition() const
{
    return juce::String (cursorPosition.yDegree.value) + " :: " + juce::String (cursorPosition.xTimepoint.value);
}

bool Cursor::isInsertMode() const { return mode == Mode::insert; }

bool Cursor::isNormalMode() const { return mode == Mode::normal; }

bool Cursor::isVisualLineMode() const { return mode == Mode::visualLine; }

bool Cursor::isVisualBlockMode() const { return mode == Mode::visualBlock; }

void Cursor::increaseTimelineStepSize() { getSelectedSequence().increaseTimelineStepSize(); }

void Cursor::decreaseTimelineStepSize() { getSelectedSequence().decreaseTimelineStepSize(); }

const std::vector<Position>& Cursor::getVisualSelectionPositions() const
{
    return visualSelection.getPositions();
}

void Cursor::toggleLineMode() { visualSelection.toggleLineMode(); }

Position Cursor::getVisualSelectionOpposite()
{
    return visualSelection.getOppositeCorner (cursorPosition);
}

std::vector<std::reference_wrapper<std::unique_ptr<Note>>> Cursor::findNotesAtPosition (Position& p, Timeline& t, Scale& s)
{
    auto minTime = p.xTimepoint.value;
    auto maxTime = p.xTimepoint.value + t.getStepSize();
    auto minDegree = p.yDegree.value;
    auto maxDegree = p.yDegree.value + s.getSmallestStepSize();

    return getSelectedSequence().findNotes (minTime, maxTime, minDegree, maxDegree);
}

std::vector<std::reference_wrapper<std::unique_ptr<Note>>> Cursor::findNotesAtCursor()
{
    return getSelectedSequence().findNotes (cursorPosition.xTimepoint.value,
                                            cursorPosition.xTimepoint.value + getCurrentTimeline().getStepSize(),
                                            cursorPosition.yDegree.value,
                                            cursorPosition.yDegree.value);
}

std::vector<std::reference_wrapper<std::unique_ptr<Note>>> Cursor::findNotesInCursorSelection()
{
    return getSelectedSequence().findNotes (visualSelection.getEarliestPosition().xTimepoint.value,
                                            visualSelection.getLatestPosition().xTimepoint.value + getCurrentTimeline().getStepSize(),
                                            visualSelection.getLowestPosition().yDegree.value,
                                            visualSelection.getHighestPosition().yDegree.value);
}

std::vector<std::reference_wrapper<std::unique_ptr<Note>>> Cursor::findNotesForCursorMode()
{
    if (isNormalMode() || isInsertMode())
    {
        return getSelectedSequence().findNotes (cursorPosition.xTimepoint.value,
                                                cursorPosition.xTimepoint.value + getCurrentTimeline().getStepSize(),
                                                cursorPosition.yDegree.value,
                                                cursorPosition.yDegree.value);
    }
    else if (isVisualBlockMode() || isVisualLineMode())
    {
        return getSelectedSequence().findNotes (visualSelection.getEarliestPosition().xTimepoint.value,
                                                visualSelection.getLatestPosition().xTimepoint.value + getCurrentTimeline().getStepSize(),
                                                visualSelection.getLowestPosition().yDegree.value,
                                                visualSelection.getHighestPosition().yDegree.value);
    }

    return {};
}

int Cursor::addModifier (ModifierType t)
{
    auto notes = findNotesForCursorMode();

    Modifier m = Modifier { t };

    for (auto& note : notes)
    {
        (*note.get()).addModifier (m);
    }

    return static_cast<int> (notes.size());
}

int Cursor::removeModifier (ModifierType t)
{
    auto notes = findNotesForCursorMode();

    for (auto& note : notes)
    {
        if (auto mod = (*note.get()).getModifier (t))
        {
            (*note.get()).removeModifier (mod->getType());
        }
    }

    return static_cast<int> (notes.size());
}

const Timeline& Cursor::getCurrentTimeline() const
{
    return getSelectedSequence().getTimeline();
}

const Scale& Cursor::getCurrentScale() const
{
    return getSelectedSequence().getScale();
}

Timeline& Cursor::getCurrentTimeline()
{
    return getSelectedSequence().getTimeline();
}

Scale& Cursor::getCurrentScale()
{
    return getSelectedSequence().getScale();
}
