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
        selectStep(sequence->steps[0].get(), 0);
        selectNote(0);
    }
};

void Cursor::selectStep(Step* s, size_t sIndex)
{
    selectedStepIndex = sIndex;
};

void Cursor::selectNote(size_t nIndex)
{
    selectedNoteIndex = nIndex;
};



//Step& Cursor::getCurrentStep() {
//    if (selectedStepIndex < sequence->steps.size()) {
//        return &sequence->steps[selectedStepIndex].get();
//    }
//    return nullptr;
//}

bool Cursor::isStepSelected(const Step& step) const
{
    //TODO check for bounds
    Step* selectedStep = sequence->steps[selectedStepIndex].get();
    
    if (selectedStep == nullptr)
        return false;

    return selectedStep == &step;
}

bool Cursor::isNoteSelected(const Note& note) const
{
    //TODO check for bounds
    Note* selectedNote = sequence->steps[selectedStepIndex].get()->notes[selectedNoteIndex].get();
    
    if (selectedNote == nullptr)
        return false;

    return selectedNote == &note;
}

bool Cursor::isStepVisuallySelected(const Step& step) const
{
    if (selectedStep == nullptr)
        return false;
    
    return selectedStep == &step;
}

void Cursor::moveLeft()
{
    if (sequence == nullptr)
        return;
    
    if (selectedStepIndex > 0)
        selectedStepIndex--;
        selectedStep = sequence->steps[selectedStepIndex].get();
        juce::Logger::writeToLog("selected note: " + juce::String(selectedNoteIndex));
};

void Cursor::moveRight()
{
    if (sequence == nullptr)
        return;
    
    if (selectedStepIndex < sequence->steps.size() - 1)
        selectedStepIndex++;
        selectedStep = sequence->steps[selectedStepIndex].get();
};

void Cursor::moveDown()
{
    if (sequence == nullptr)
        return;
    
    if (selectedStep == nullptr)
        return;
    
    selectedStep->stepDown(selectedNoteIndex);
}

void Cursor::moveUp()
{
    if (sequence == nullptr)
        return;
    
    if (selectedStep == nullptr)
        return;
    
    selectedStep->stepUp(selectedNoteIndex);
}

void Cursor::jumpToStart()
{
    if (sequence == nullptr)
        return;
    
    selectedStepIndex = 0;
    selectedStep = sequence->steps[selectedStepIndex].get();
}

void Cursor::jumpToEnd()
{
    if (sequence == nullptr)
        return;
    
    selectedStepIndex = sequence->steps.size();
    selectedStep = sequence->steps[selectedStepIndex - 1].get();
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
    sequence->steps[selectedStepIndex]->addNote();
}

void Cursor::removeNote()
{
    sequence->steps[selectedStepIndex]->removeNote(0);
}

size_t Cursor::getStepIndex()
{
    return selectedStepIndex;
}

size_t Cursor::getNoteIndex()
{
    return selectedNoteIndex;
}
