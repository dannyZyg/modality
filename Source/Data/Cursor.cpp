/*
  ==============================================================================

    Cursor.cpp
    Created: 13 Jan 2025 3:09:08pm
    Author:  Danny Keig

  ==============================================================================
*/

#include "Cursor.h"
#include "Data/Selection.h"
#include "juce_core/system/juce_PlatformDefs.h"

Cursor::Cursor() : randomGenerator (std::random_device()())
{
    createSequence();
    selectSequence (0);
}

Cursor::~Cursor() {}

void Cursor::createSequence()
{
    auto sequence = std::make_unique<Sequence>();
    sequences.emplace_back (std::move (sequence));
}

void Cursor::selectSequence (size_t index)
{
    selectedSeqIndex = index;
    selectNote (0);
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
    for (const Position& pos : getVisualSelectionPositions())
    {
        for (auto& note : getSelectedSequence().notes)
        {
            if (Division::isEqual (pos.xTime.value, note->getStartTime())
                && Division::isEqual (pos.yDegree.value, note->getDegree()))
            {
                switch (d)
                {
                    case Direction::left:
                        note->shiftEarlier (timeline.getStepSize());
                        break;
                    case Direction::right:
                        note->shiftLater (timeline.getStepSize());
                        break;
                    case Direction::up:
                        note->shiftDegreeUp();
                        break;
                    case Direction::down:
                        note->shiftDegreeDown();
                        break;
                    default:
                        break;
                }
            }
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
            cursorPosition.xTime = timeline.getPrevStep (cursorPosition.xTime);
            break;
        case Direction::right:
            cursorPosition.xTime = timeline.getNextStep (cursorPosition.xTime);
            break;
        case Direction::up:
            cursorPosition.yDegree = scale.getHigher (cursorPosition.yDegree);
            break;
        case Direction::down:
            cursorPosition.yDegree = scale.getLower (cursorPosition.yDegree);
            break;
        default:
            break;
    }

    if (moveMode == Selection::MoveMode::shift)
    {
        double stepSize = d == Direction::left || d == Direction::right ? timeline.getStepSize() : scale.getStepSize();
        visualSelection.moveSelection (stepSize, d);
    }
    else if (moveMode == Selection::MoveMode::extend)
    {
        if (isVisualLineMode())
        {
            visualSelection.addToVisualLineSelection (cursorPosition, timeline, scale);
        }

        if (isVisualBlockMode())
        {
            visualSelection.addToVisualBlockSelection (cursorPosition);
        }
    }
}

void Cursor::jumpToStart()
{
    cursorPosition.xTime.value = timeline.getLowerBound();
}

void Cursor::jumpToEnd()
{
    cursorPosition.xTime.value = timeline.getUpperBound() - timeline.getStepSize();
}

void Cursor::jumpForwardBeat()
{
    cursorPosition.xTime.value = timeline.getNextStep (cursorPosition.xTime.value, Division::whole).value;
}

void Cursor::jumpBackBeat()
{
    cursorPosition.xTime.value = timeline.getPrevStep (cursorPosition.xTime.value, Division::whole).value;
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
    visualSelection.addToVisualLineSelection (cursorPosition, timeline, scale);
}

void Cursor::enableVisualBlockMode()
{
    mode = Mode::visualBlock;
    visualSelection.addToVisualBlockSelection (cursorPosition);
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

/* void Cursor::nextNearestNote() */
/* { */
/**/
/* } */
/**/
/* void Cursor::prevNearestNote() */
/* { */
/**/
/* } */

void Cursor::previewNote()
{
}

void Cursor::previewStep()
{
}

Sequence& Cursor::getSequence (size_t index) const
{
    if (index >= sequences.size())
    {
        std::string m = "Seq index out of bounds: " + std::to_string (index);
        juce::Logger::writeToLog (m);
        throw std::out_of_range (m);
    }
    return *sequences[index];
}

Sequence& Cursor::getSelectedSequence() const
{
    return getSequence (selectedSeqIndex);
}

std::vector<MidiNote> Cursor::extractMidiSequence (size_t seqIndex)
{
    std::vector<MidiNote> midiClip;

    float tempo = 120.0;

    for (auto& n : getSequence (seqIndex).notes)
    {
        auto midi = n->asMidiNote (timeline, scale, tempo);

        if (midi)
        {
            midiClip.emplace_back (*midi);
        }
    }
    return midiClip;
}

void Cursor::insertNote()
{
    auto note = std::make_unique<Note> (cursorPosition.yDegree.value, cursorPosition.xTime.value, timeline.getStepSize());
    getSelectedSequence().notes.emplace_back (std::move (note));
}

void Cursor::removeNotesAtCursor()
{
    if (isNormalMode() || isInsertMode())
    {
        double degMin = cursorPosition.yDegree.value;
        double degMax = degMin;
        double timeStart = cursorPosition.xTime.value;
        double timeEnd = timeStart + timeline.getStepSize();

        getSelectedSequence().removeNotes (timeStart, timeEnd, degMin, degMax);
    }
    else if (isVisualLineMode() || isVisualBlockMode())
    {
        double degMin = visualSelection.getLowestPosition().yDegree.value;
        double degMax = visualSelection.getHighestPosition().yDegree.value;
        double timeStart = visualSelection.getEarliestPosition().xTime.value;
        double timeEnd = visualSelection.getLatestPosition().xTime.value + timeline.getStepSize();

        getSelectedSequence().removeNotes (timeStart, timeEnd, degMin, degMax);
    }
}

const juce::String Cursor::readableCursorPosition() const
{
    return juce::String (cursorPosition.yDegree.value) + " :: " + juce::String (cursorPosition.xTime.value);
}

bool Cursor::isInsertMode() const { return mode == Mode::insert; }
bool Cursor::isNormalMode() const { return mode == Mode::normal; }
bool Cursor::isVisualLineMode() const { return mode == Mode::visualLine; }
bool Cursor::isVisualBlockMode() const { return mode == Mode::visualBlock; }

void Cursor::increaseTimelineStepSize() { timeline.increaseStepSize(); }
void Cursor::decreaseTimelineStepSize() { timeline.decreaseStepSize(); }

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
    auto minTime = p.xTime.value;
    auto maxTime = p.xTime.value + t.getStepSize();
    auto minDegree = p.yDegree.value;
    auto maxDegree = p.yDegree.value + s.getStepSize();

    return getSelectedSequence().findNotes (minTime, maxTime, minDegree, maxDegree);
}

std::vector<std::reference_wrapper<std::unique_ptr<Note>>> Cursor::findNotesAtCursor()
{
    return getSelectedSequence().findNotes (cursorPosition.xTime.value,
                                            cursorPosition.xTime.value + timeline.getStepSize(),
                                            cursorPosition.yDegree.value,
                                            cursorPosition.yDegree.value);
}

std::vector<std::reference_wrapper<std::unique_ptr<Note>>> Cursor::findNotesInCursorSelection()
{
    return getSelectedSequence().findNotes (visualSelection.getEarliestPosition().xTime.value,
                                            visualSelection.getLatestPosition().xTime.value + timeline.getStepSize(),
                                            visualSelection.getLowestPosition().yDegree.value,
                                            visualSelection.getHighestPosition().yDegree.value);
}

std::vector<std::reference_wrapper<std::unique_ptr<Note>>> Cursor::findNotesForCursorMode()
{
    if (isNormalMode() || isInsertMode())
    {
        return getSelectedSequence().findNotes (cursorPosition.xTime.value,
                                                cursorPosition.xTime.value + timeline.getStepSize(),
                                                cursorPosition.yDegree.value,
                                                cursorPosition.yDegree.value);
    }
    else if (isVisualBlockMode() || isVisualLineMode())
    {
        return getSelectedSequence().findNotes (visualSelection.getEarliestPosition().xTime.value,
                                                visualSelection.getLatestPosition().xTime.value + timeline.getStepSize(),
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
            (*note.get()).removeModifier (*mod);
        }
    }

    return static_cast<int> (notes.size());
}
