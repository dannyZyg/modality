/*
  ==============================================================================

    Cursor.cpp
    Created: 13 Jan 2025 3:09:08pm
    Author:  Danny Keig

  ==============================================================================
*/

#include "Cursor.h"

Cursor::Cursor() {};

Cursor::~Cursor() {};

void Cursor::selectSequence(Sequence* seq)
{
    sequence = seq;
    
    // Pass through the callback for individual steps to use to see if they are active
    if (sequence) {
        sequence->setIsSelectedCallback([this](const Step& step) {
            return isStepSelected(step);
        },[this](const Note& note) {
            return isNoteSelected(note);
        }
        );
        selectStep(0);
        selectNote(0);
    }
};

void Cursor::selectStep(size_t sIndex)
{
    selectedStepIndex = sIndex;
};

void Cursor::selectNote(size_t nIndex)
{
    selectedNoteIndex = nIndex;
};

bool Cursor::isStepSelected(const Step& step) const
{
    Step& selectedStep = sequence->getStep(selectedStepIndex);
    return &selectedStep == &step;
}

bool Cursor::isNoteSelected(const Note& note) const
{
    Step& selectedStep = sequence->getStep(selectedStepIndex);
    Note& selectedNote = selectedStep.getNote(selectedNoteIndex);
    return &selectedNote == &note;
}

void Cursor::moveLeft()
{
    if (sequence == nullptr)
        return;
    
    // Only shift left if we won't go below 0
    if (selectedStepIndex > 0) {
        selectedStepIndex--;
        
        // Reset note index to 0 of the next Step
        selectedNoteIndex = 0;
    }
};

void Cursor::moveRight()
{
    if (sequence == nullptr)
        return;
    
    // Only shift right if we won't go out of bounds
    if (selectedStepIndex < sequence->steps.size() - 1) {
        selectedStepIndex++;
        
        // Reset note index to 0 of the next Step
        selectedNoteIndex = 0;
    }
};

void Cursor::moveDown()
{
    if (sequence == nullptr)
        return;
    
    Step& selectedStep = sequence->getStep(selectedStepIndex);
    selectedStep.selectedNoteDown(selectedNoteIndex);
}

void Cursor::moveUp()
{
    if (sequence == nullptr)
        return;
    
    Step& selectedStep = sequence->getStep(selectedStepIndex);
    selectedStep.selectedNoteUp(selectedNoteIndex);
}

void Cursor::jumpToStart()
{
    if (sequence == nullptr)
        return;
    
    selectedStepIndex = 0;
    selectedNoteIndex = 0;
}

void Cursor::jumpToEnd()
{
    if (sequence == nullptr)
        return;
    
    selectedStepIndex = sequence->steps.size() - 1;
    selectedNoteIndex = 0;
}

void Cursor::enableNormalMode()
{
    mode = Mode::normal;
};

void Cursor::enableVisualMode()
{
    mode = Mode::visual;
};

void Cursor::toggleStepMute()
{
    if (sequence == nullptr)
        return;
    
    sequence->steps[selectedStepIndex]->toggleMute();
};

Mode Cursor::getMode()
{
    return mode;
};

juce::String Cursor::getModeName()
{
    return modeToString(mode);
};

constexpr const char* Cursor::modeToString(Mode m) throw()
{
    switch (m)
    {
        case Mode::normal: return "Normal Mode";
        case Mode::visual: return "Visual Mode";
        default: throw std::invalid_argument("Unimplemented item");
    }
};

void Cursor::addNote()
{
    Step& step = sequence->getStep(selectedStepIndex);
    step.addNote();
}

void Cursor::removeNote()
{
    Step& step = sequence->getStep(selectedStepIndex);
    step.removeNote(selectedNoteIndex);
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
    selectedNoteIndex = sequence->nextNoteIndexInStep(selectedStepIndex, selectedNoteIndex);
}

void Cursor::prevNoteInStep()
{
    selectedNoteIndex = sequence->prevNoteIndexInStep(selectedStepIndex, selectedNoteIndex);
}

void Cursor::previewNote()
{
    Step& step = sequence->getStep(selectedStepIndex);
    //step.playNote(step);
}

void Cursor::previewStep()
{
    Step& step = sequence->getStep(selectedStepIndex);
    step.playStep();
}
