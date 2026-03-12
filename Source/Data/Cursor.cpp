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
#include "Data/Timeline.h"
#include "juce_data_structures/juce_data_structures.h"

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
    // Check if the selection can move before doing anything
    if (! shouldWrap && ! visualSelection.canSelectionMove (getCurrentTimeline(), getCurrentScale(), d))
        return;

    moveNotesInSelection (d);
    move (d, Selection::MoveMode::shift);
}

void Cursor::moveNotesInSelection (Direction d)
{
    undoManager.beginNewTransaction ("moveNotesInSelection");
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
                TimePoint currentTime (note->getStartTime());
                TimePoint newTime = seq.getTimeline().getPrevStep (currentTime, shouldWrap);
                note->setStartTime (newTime.value, &undoManager);
                break;
            }
            case Direction::right:
            {
                TimePoint currentTime (note->getStartTime());
                TimePoint newTime = seq.getTimeline().getNextStep (currentTime, shouldWrap);
                note->setStartTime (newTime.value, &undoManager);
                break;
            }
            case Direction::up:
            {
                Degree currentDeg = note->getDegree();
                Degree higherDeg = seq.getScale().getHigher (currentDeg, shouldWrap);
                note->setDegree (higherDeg.value, &undoManager);
                break;
            }
            case Direction::down:
            {
                Degree currentDeg = note->getDegree();
                Degree lowerDeg = seq.getScale().getLower (currentDeg, shouldWrap);
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
void Cursor::move (Direction dir, Selection::MoveMode moveMode)
{
    switch (dir)
    {
        case Direction::left:
            cursorPosition.xTimepoint = getCurrentTimeline().getPrevStep (cursorPosition.xTimepoint, shouldWrap);
            break;
        case Direction::right:
            cursorPosition.xTimepoint = getCurrentTimeline().getNextStep (cursorPosition.xTimepoint, shouldWrap);
            break;
        case Direction::up:
            cursorPosition.yDegree = getCurrentScale().getHigher (cursorPosition.yDegree, shouldWrap);
            break;
        case Direction::down:
            cursorPosition.yDegree = getCurrentScale().getLower (cursorPosition.yDegree, shouldWrap);
            break;
        default:
            break;
    }

    if (moveMode == Selection::MoveMode::shift)
    {
        visualSelection.moveSelection (getCurrentTimeline(), getCurrentScale(), dir, shouldWrap);
    }
    else if (moveMode == Selection::MoveMode::extend)
    {
        if (isVisualLineMode())
        {
            visualSelection.addToVisualLineSelection (cursorPosition, getCurrentTimeline(), getCurrentScale());
        }

        if (isVisualBlockMode())
        {
            visualSelection.addToVisualBlockSelection (cursorPosition, getCurrentTimeline(), getCurrentScale(), shouldWrap);
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
    cursorPosition.xTimepoint = getCurrentTimeline().getNextStep (cursorPosition.xTimepoint.value, Division::whole, shouldWrap);
}

void Cursor::jumpBackBeat()
{
    cursorPosition.xTimepoint = getCurrentTimeline().getPrevStep (cursorPosition.xTimepoint.value, Division::whole, shouldWrap);
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
    visualSelection.addToVisualBlockSelection (cursorPosition, getCurrentTimeline(), getCurrentScale(), shouldWrap);
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

void Cursor::yankNotes (double originTimepoint, double originDegree)
{
    auto notes = findNotesForCursorMode();

    clipboard = juce::ValueTree (CursorIDs::YankModeNotes);

    juce::ValueTree yankedNotes (CursorIDs::YankedNotes);
    clipboard.addChild (yankedNotes, -1, nullptr);

    for (auto note : notes)
    {
        juce::ValueTree yankedNoteState = note.get()->getState().createCopy();

        // calculate time offset
        double noteStartTime = note.get()->getStartTime();
        double timeOffset = noteStartTime - originTimepoint;
        yankedNoteState.setProperty (CursorIDs::YankedNoteTimepointOffset, timeOffset, nullptr);

        // calculate degree offset
        Degree noteDegree = note.get()->getDegree();
        int degreeSteps = getCurrentScale().getStepsBetween (originDegree, noteDegree);
        yankedNoteState.setProperty (CursorIDs::YankedNoteDegreeOffset, degreeSteps, nullptr);

        yankedNotes.addChild (yankedNoteState, -1, nullptr);
    }
}

void Cursor::yank (juce::Identifier yankMode)
{
    if (yankMode == CursorIDs::YankModeNotes)
    {
        // TODO when in visual modes this should be the top left vector of the selection.
        yankNotes (cursorPosition.xTimepoint.value, cursorPosition.yDegree.value);
    }
    enableNormalMode();
}

void Cursor::paste()
{
    if (! clipboard.hasType (CursorIDs::YankModeNotes))
        return;

    auto yankedNotes = clipboard.getChildWithName (CursorIDs::YankedNotes);

    if (! yankedNotes.isValid())
        return;

    undoManager.beginNewTransaction ("paste");

    for (auto note : yankedNotes)
    {
        double timepointOffset = note.getProperty (CursorIDs::YankedNoteTimepointOffset);
        int degreeOffset = note.getProperty (CursorIDs::YankedNoteDegreeOffset);

        double newStartTime = cursorPosition.xTimepoint.value + timepointOffset;

        if (shouldWrap)
        {
            newStartTime = getCurrentTimeline().wrapTime (newStartTime);
        }
        else
        {
            if (! getCurrentTimeline().isWithinBounds (newStartTime))
                continue;
        }

        std::optional<Degree> newDegreeOpt = getCurrentScale().applySteps (cursorPosition.yDegree, degreeOffset, shouldWrap);

        if (! newDegreeOpt.has_value())
            continue;

        Degree newDegree = newDegreeOpt.value();

        juce::ValueTree pastedNote = note.createCopy();
        pastedNote.setProperty (NoteIDs::StartTime, newStartTime, nullptr);
        pastedNote.setProperty (NoteIDs::Degree, newDegree.value, nullptr);
        // remove properties related to the yank
        pastedNote.removeProperty (CursorIDs::YankedNoteTimepointOffset, nullptr);
        pastedNote.removeProperty (CursorIDs::YankedNoteDegreeOffset, nullptr);

        getSelectedSequence().insertNote (pastedNote, &undoManager);
    }
}

juce::ValueTree Cursor::getClipboard()
{
    return clipboard;
}
