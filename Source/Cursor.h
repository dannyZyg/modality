/*
  ==============================================================================

    Cursor.h
    Created: 13 Jan 2025 3:09:08pm
    Author:  Danny Keig

  ==============================================================================
*/

# include <JuceHeader.h>
# include "Step.h"
# include "Sequence.h"
#pragma once

enum class Mode { normal, visual };

class Cursor
{
    
public:
    Cursor();
    ~Cursor();
    
    void selectSequence(Sequence* seq);
    
    void setVisualSelection();
    void jumpToStart();
    void jumpToEnd();
    void moveRight();
    void moveLeft();
    void moveDown();
    void moveUp();
    void enableNormalMode();
    void enableVisualMode();
    Mode getMode();
    juce::String getModeName();
    constexpr const char* modeToString(Mode m) throw();
    
    bool isStepSelected(const Step& step) const;

private:
    Mode mode = Mode::normal;

    Sequence* sequence = nullptr;
    Step* selectedStep = nullptr;
    std::vector<Step*> visualSelection;

    void selectStep(Step* s, unsigned long sIndex);
    unsigned long selectedStepIndex = 0;
};
