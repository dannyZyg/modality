/*
  ==============================================================================

    Cursor.cpp
    Created: 13 Jan 2025 3:09:08pm
    Author:  Danny Keig

  ==============================================================================
*/

#include "Cursor.h"

Cursor::Cursor() {
  createSequence();
  selectSequence(0);
}

Cursor::~Cursor() {}

void Cursor::createSequence()
{
    auto sequence = std::make_unique<Sequence>([this](const Sequence& s) { return isSequenceSelected(s); });

    for (int i = 0; i < sequence->lengthBeats * sequence->stepsPerBeat; ++i) {
        auto step = std::make_unique<Step>([this](const Step& s) { return isStepSelected(s); });

        step->addNote([this](const Note& note) { return isNoteSelected(note); });

        // We must wait until we have finished modifying the step (adding notes) before adding this to the seq
        // otherwise we are trying to modify something that is no longer available in this scope
        sequence->steps.emplace_back(std::move(step));
    }

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


void Cursor::moveLeft()
{
    // Only shift left if we won't go below 0
    if (selectedStepIndex > 0) {
        selectedStepIndex--;

        // Reset note index to 0 of the next Step
        selectedNoteIndex = 0;
    }
}

void Cursor::moveRight()
{
    // Only shift right if we won't go out of bounds
    if (selectedStepIndex < getSelectedSequence().steps.size() - 1) {
        selectedStepIndex++;

        // Reset note index to 0 of the next Step
        selectedNoteIndex = 0;
    }
}

void Cursor::moveDown()
{
    Step& selectedStep = getSelectedSequence().getStep(selectedStepIndex);
    selectedStep.selectedNoteDown(selectedNoteIndex);
}

void Cursor::moveUp()
{
    Step& selectedStep = getSelectedSequence().getStep(selectedStepIndex);
    selectedStep.selectedNoteUp(selectedNoteIndex);
}

void Cursor::jumpToStart()
{
    selectedStepIndex = 0;
    selectedNoteIndex = 0;
}

void Cursor::jumpToEnd()
{
    selectedStepIndex = getSelectedSequence().steps.size() - 1;
    selectedNoteIndex = 0;
}

void Cursor::enableNormalMode()
{
    mode = Mode::normal;
}

void Cursor::enableVisualMode()
{
    mode = Mode::visual;
}

void Cursor::toggleStepMute()
{
    getSelectedSequence().steps[selectedStepIndex]->toggleMute();
}

Mode Cursor::getMode()
{
    return mode;
}

juce::String Cursor::getModeName()
{
    return modeToString(mode);
}

constexpr const char* Cursor::modeToString(Mode m) throw()
{
    switch (m)
    {
        case Mode::normal: return "Normal Mode";
        case Mode::visual: return "Visual Mode";
        default: throw std::invalid_argument("Unimplemented item");
    }
}

void Cursor::addNote()
{
    Step& step = getSelectedSequence().getStep(selectedStepIndex);
    selectedNoteIndex = step.addNote(
        [this](const Note& note) { return isNoteSelected(note); }
    );

    juce::Logger::writeToLog("selectedNoteIndex is: " + juce::String(selectedNoteIndex));
}

void Cursor::removeNote()
{
    Step& step = getSelectedSequence().getStep(selectedStepIndex);
    selectedNoteIndex = step.removeNote(selectedNoteIndex);
}

size_t Cursor::getStepIndex()
{
    return selectedStepIndex;
}

size_t Cursor::getNoteIndex()
{
    return selectedNoteIndex;
}

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
