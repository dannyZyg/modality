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
#include "Data/Selectable.h"


//==============================================================================
/*
*/
class Step  : public juce::ChangeBroadcaster, public Selectable<Step>
{
public:
    using Selectable<Step>::Selectable;
    ~Step();

    void selectedNoteUp(size_t nIndex);
    void selectedNoteDown(size_t nIndex);
    void playNote(int degree);
    void playStep();
    void setMidiOutput();
    void toggleMute();
    bool isVisuallySelected = false;

    bool addNote(std::function<bool(const Note&)> isSelectedCallback);
    bool removeNote(size_t noteIndex);

    std::vector<std::unique_ptr<Note>> notes;

    Note& getNote(size_t index);

private:
    const int MAX_POLYPHONY = 6;
    enum class MuteMode { muted, unmuted };
    MuteMode muteMode = MuteMode::unmuted;
    //int initialNumNotes = 1 + std::rand() % 4;
    int initialNumNotes = 1; // + std::rand() % 4;
};
