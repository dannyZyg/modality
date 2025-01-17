/*
  ==============================================================================

    Step.h
    Created: 10 Jan 2025 11:01:43am
    Author:  Danny Keig

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <cstdlib>
#include "Note.h"


//==============================================================================
/*
*/
class Step  : public juce::Component
{
public:
    Step();
    ~Step();

    void selectedNoteUp(size_t nIndex);
    void selectedNoteDown(size_t nIndex);
    void playNote(int degree);
    void playStep();
    void setMidiOutput();
    void toggleMute();
    bool isVisuallySelected = false;
    
    bool isSelected() const;
    void setIsSelectedCallback(std::function<bool(const Step&)> callback);

    void addNote();
    void removeNote(int noteIndex);
    std::vector<std::unique_ptr<Note>> notes;
    
    Note& getNote(size_t index);
    
private:
    std::function<bool(const Step&)> isSelectedCallback = nullptr;
    enum class MuteMode { muted, unmuted };
    MuteMode muteMode = MuteMode::unmuted;
    int initialNumNotes = 1 + std::rand() % 4;
};
