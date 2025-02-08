/*
  ==============================================================================

    Cursor.cpp
    Created: 13 Jan 2025 3:09:08pm
    Author:  Danny Keig

  ==============================================================================
*/

#include "Cursor.h"
#include "juce_core/system/juce_PlatformDefs.h"

Cursor::Cursor() : randomGenerator(std::random_device()()) {

  createSequence();
  selectSequence(0);
}

Cursor::~Cursor() {}

void Cursor::createSequence()
{
    auto sequence = std::make_unique<Sequence>([this](const Sequence& s) { return isSequenceSelected(s); });
    sequences.emplace_back(std::move(sequence));
}

void Cursor::selectSequence(size_t index)
{
    selectedSeqIndex = index;
    selectStep(0);
    selectNote(0);
}

void Cursor::selectStep(size_t sIndex)
{
    selectedStepIndex = sIndex;
}

void Cursor::selectNote(size_t nIndex)
{
    selectedNoteIndex = nIndex;
}

bool Cursor::isSequenceSelected(const Sequence& otherSequence) const
{
    Sequence& selectedSequence = getSelectedSequence();
    return &selectedSequence == &otherSequence;
}

bool Cursor::isStepSelected(const Step& step) const
{
    return &getSelectedSequence().getStep(selectedStepIndex) == &step;
}

bool Cursor::isNoteSelected(const Note& note) const
{
    return &note == &getSelectedSequence().getStep(selectedStepIndex).getNote(selectedNoteIndex);
}

void Cursor::moveCursorSelection(Direction d)
{
    moveNotesInSelection(d);

    switch(d)
    {
        case Direction::left:
            visualSelection.moveSelection(timeline.getStepSize(), d);
            moveLeft();
            break;
        case Direction::right:
            visualSelection.moveSelection(timeline.getStepSize(), d);
            moveRight();
            break;
        case Direction::up:
            visualSelection.moveSelection(scale.getStepSize(), d);
            moveUp();
            break;
        case Direction::down:
            visualSelection.moveSelection(scale.getStepSize(), d);
            moveDown();
            break;
        default: break;
    }
}

void Cursor::moveNotesInSelection(Direction d)
{
    for (const Position& pos: getVisualSelectionPositions()) {
        for (auto& note : getSelectedSequence().notes) {
            if (Division::isEqual(pos.xTime.value, note->getStartTime())
                && Division::isEqual(pos.yDegree.value, note->getDegree()))
            {
                switch (d)
                {
                    case Direction::left:
                        note->shiftEarlier(timeline.getStepSize());
                        break;
                    case Direction::right:
                        note->shiftLater(timeline.getStepSize());
                        break;
                    case Direction::up:
                        note->shiftDegreeUp();
                        break;
                    case Direction::down:
                        note->shiftDegreeDown();
                        break;
                    default: break;
                }
            }
        }
    }
}

void Cursor::moveLeft()
{
    cursorPosition.xTime = timeline.getPrevStep(cursorPosition.xTime);

    if (isVisualLineMode()) {
        visualSelection.addToSelection(cursorPosition);
    }

    if (isVisualBlockMode()) {
        visualSelection.addToBlockSelection(cursorPosition);
    }
}

void Cursor::moveRight()
{
    cursorPosition.xTime = timeline.getNextStep(cursorPosition.xTime);

    if (isVisualLineMode()) {
        visualSelection.addToSelection(cursorPosition);
    }

    if (isVisualBlockMode()) {
        visualSelection.addToBlockSelection(cursorPosition);
    }
}

void Cursor::moveDown()
{
    cursorPosition.yDegree = scale.getLower(cursorPosition.yDegree);

    if (isVisualLineMode()) {
        visualSelection.addToSelection(cursorPosition);
    }

    if (isVisualBlockMode()) {
        visualSelection.addToBlockSelection(cursorPosition);
    }
}

void Cursor::moveUp()
{
    cursorPosition.yDegree = scale.getHigher(cursorPosition.yDegree);

    if (isVisualLineMode()) {
        visualSelection.addToSelection(cursorPosition);
    }

    if (isVisualBlockMode()) {
        visualSelection.addToBlockSelection(cursorPosition);
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
    cursorPosition.xTime.value = timeline.getNextStep(cursorPosition.xTime.value, Division::quarter).value;
}

void Cursor::jumpBackBeat()
{
    cursorPosition.xTime.value = timeline.getPrevStep(cursorPosition.xTime.value, Division::quarter).value;
}

void Cursor::enableNormalMode()
{
    if (isVisualLineMode() || isVisualBlockMode()) {
        visualSelection.clear();
    }

    mode = Mode::normal;
}

void Cursor::enableVisualLineMode()
{
    mode = Mode::visualLine;

    visualSelection.addToVisualLineSelection(cursorPosition, Selection::VisualLineMode::vertical, timeline, scale);
}

void Cursor::enableVisualBlockMode()
{
    mode = Mode::visualBlock;
    visualSelection.addToSelection(cursorPosition);
}

void Cursor::enableInsertMode()
{
    mode = Mode::insert;
}

void Cursor::toggleStepMute()
{
    getSelectedSequence().steps[selectedStepIndex]->toggleMute();
}

Mode Cursor::getMode()
{
    return mode;
}

const juce::String Cursor::getModeName() const
{
    return modeToString(mode);
}

constexpr const char* Cursor::modeToString(Mode m) const
{
    switch (m)
    {
        case Mode::normal: return "NORMAL";
        case Mode::visualBlock: return "V-BLOCK";
        case Mode::visualLine: return "V-LINE";
        case Mode::insert: return "INSERT";
        default: return "UNKOWN";
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

void Cursor::nextNoteInStep()
{
    size_t n = getSelectedSequence().nextNoteIndexInStep(selectedStepIndex, selectedNoteIndex);
    juce::Logger::writeToLog("Next note is: " + juce::String(n));
    selectedNoteIndex = getSelectedSequence().nextNoteIndexInStep(selectedStepIndex, selectedNoteIndex);
}

void Cursor::prevNoteInStep()
{
    selectedNoteIndex = getSelectedSequence().prevNoteIndexInStep(selectedStepIndex, selectedNoteIndex);
}

void Cursor::previewNote()
{
    Step& step = getSelectedSequence().getStep(selectedStepIndex);
    //step.playNote(step);
}

void Cursor::previewStep()
{
    Step& step = getSelectedSequence().getStep(selectedStepIndex);
    step.playStep();
}

Sequence& Cursor::getSequence(size_t index) const
{
    if (index >= sequences.size()) {
        std::string m = "Seq index out of bounds: " + std::to_string(index);
        juce::Logger::writeToLog(m);
        throw std::out_of_range(m);
    }
    return *sequences[index];
}

Sequence& Cursor::getSelectedSequence() const
{
    return getSequence(selectedSeqIndex);
}

std::vector<Sequence::MidiNote> Cursor::extractMidiSequence(size_t seqIndex)
{
    std::vector<Sequence::MidiNote> midiClip;

    float tempo = 120.0;

    for (auto& n : getSequence(seqIndex).notes) {
        auto mod = n->getModifier(ModifierType::randomTrigger);


        if (mod) {
            auto probability = mod->getModifierValue("percentChanceTriggerValue");
            std::bernoulli_distribution d(any_cast<double>(probability));
            /* DBG("PRobability: " << any_cast<double>(probability)); */
            /* DBG("MOD Exists"); */
            if (d(randomGenerator)) {
                /* DBG("YES"); */
            }
        }

        double time = timeline.convertBarPositionToSeconds(n->getStartTime(), tempo);

        midiClip.emplace_back(time, 64 + n->getDegree(), 100, 1 * 0.9);
    }
    return midiClip;
}

void Cursor::insertNote()
{
    auto note = std::make_unique<Note>(cursorPosition.yDegree.value, cursorPosition.xTime.value, timeline.getStepSize());
    getSelectedSequence().notes.emplace_back(std::move(note));
}

void Cursor::removeNotesAtCursor()
{
    double degMin = cursorPosition.yDegree.value;
    double degMax = degMin;
    double timeStart = cursorPosition.xTime.value;
    double timeEnd = timeStart + timeline.getStepSize();

    getSelectedSequence().removeNotes(timeStart, timeEnd, degMin, degMax);
}

juce::String Cursor::readableCursorPosition()
{
    return juce::String(cursorPosition.yDegree.value) + ":" + juce::String(cursorPosition.xTime.value);
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

Position Cursor::getVisualSelectionOpposite()
{
    return visualSelection.getOppositeCorner(cursorPosition);
}

std::vector<std::reference_wrapper<std::unique_ptr<Note>>> Cursor::findNotesAtPosition(Position& p, Timeline& t, Scale& s)
{
    auto minTime = p.xTime.value;
    auto maxTime = p.xTime.value + t.getStepSize();
    auto minDegree = p.yDegree.value;
    auto maxDegree = p.yDegree.value + s.getStepSize();

    return getSelectedSequence().findNotes(minTime, maxTime, minDegree, maxDegree);
}


void Cursor::addModifier()
{
    DBG("Adding modifier");
    auto notes = getSelectedSequence().findNotes(cursorPosition.xTime.value, cursorPosition.xTime.value + 0.25, 0, 10);

    Modifier m = Modifier{ModifierType::randomTrigger};

    m.setModifierValue("percentChanceTriggerValue", 0.5);

    for (auto& note : notes) {
        (*note.get()).addModifier(m);
    }
}
