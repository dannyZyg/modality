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
        });
        selectStep(sequence->steps[0].get(), 0);
    }
};

void Cursor::selectStep(Step* s, int sIndex)
{
    selectedStepIndex = sIndex;
    selectedStep = s;
};

bool Cursor::isStepSelected(const Step& step) const {
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
    
    selectedStep->stepDown();
}

void Cursor::moveUp()
{
    if (sequence == nullptr)
        return;
    
    if (selectedStep == nullptr)
        return;
    
    selectedStep->stepUp();
}

void Cursor::enableNormalMode()
{
    mode = Mode::normal;
};

void Cursor::enableVisualMode()
{
    mode = Mode::visual;
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
