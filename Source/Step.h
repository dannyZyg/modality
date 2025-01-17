/*
  ==============================================================================

    Step.h
    Created: 10 Jan 2025 11:01:43am
    Author:  Danny Keig

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Note.h"


//==============================================================================
/*
*/
class Step  : public juce::Component
{
public:
    Step();
    ~Step();

    void stepUp(size_t nIndex);
    void stepDown(size_t nIndex);
    void playNote();
    void setMidiOutput();
    void toggleMute();
    bool isVisuallySelected = false;
    
    bool isSelected() const;
    void setIsSelectedCallback(std::function<bool(const Step&)> callback);

    int stepValue = 0;
    
    void addNote();
    void removeNote(int noteIndex);
    std::vector<std::unique_ptr<Note>> notes;
private:
    std::function<bool(const Step&)> isSelectedCallback = nullptr;
    enum class MuteMode { muted, unmuted };
    MuteMode muteMode = MuteMode::unmuted;
};
