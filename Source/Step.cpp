/*
  ==============================================================================

    Step.cpp
    Created: 10 Jan 2025 11:01:43am
    Author:  Danny Keig

  ==============================================================================
*/

#include <JuceHeader.h>
#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include "Step.h"

//==============================================================================
Step::Step()
{
    for (int i = 0; i < initialNumNotes; i++) {
        addNote();
    }
}

Step::~Step()
{
}

Note& Step::getNote(size_t index)
{
    if (index >= notes.size()) {
        std::string m = "Note index out of bounds: " + std::to_string(index);
        juce::Logger::writeToLog(m);
        throw std::out_of_range(m);
    }
    return *notes[index];
}

void Step::selectedNoteUp(size_t nIndex) {
    // Move up the y axis
    auto& n = notes[nIndex];
    n->degree--;
    playNote(n->degree);
}

void Step::selectedNoteDown(size_t nIndex) {
    // Move down the y axis
    auto& n = notes[nIndex];
    n->degree++;
    playNote(n->degree);
}

bool Step::isSelected() const {
    return isSelectedCallback ? isSelectedCallback(*this) : false;
}

void Step::setIsSelectedCallback(std::function<bool (const Step &)> callback)
{
    isSelectedCallback = callback;
}

void Step::toggleMute()
{
    if (muteMode == MuteMode::muted)
        muteMode = MuteMode::unmuted;
    
    if (muteMode == MuteMode::unmuted)
        muteMode = MuteMode::muted;
}

void Step::addNote()
{
    notes.emplace_back(std::make_unique<Note>(0));
    std::cout << "adding note" << std::endl;
    sendChangeMessage();
}

void Step::removeNote(size_t noteIndex)
{
    //TODO: do we want to allow no note to exist?
    if (notes.size() == 1)
        return;
    
    if (noteIndex < notes.size()) {
        std::cout << "removing note" << std::endl;
        notes.erase(notes.begin() + noteIndex);
        sendChangeMessage();
    }
}

void Step::playNote(int degree)
{
    auto message = juce::MidiMessage::noteOn (1, 64 - degree, (juce::uint8) 100);
    auto messageOff = juce::MidiMessage::noteOff (message.getChannel(), message.getNoteNumber());
    messageOff.setTimeStamp (message.getTimeStamp() + 0.1);
    
    auto midiOutputs = juce::MidiOutput::getAvailableDevices();
    auto midiOutput = juce::MidiOutput::openDevice(midiOutputs[0].identifier);
    juce::StringArray midiOutputNames;
    
    
    midiOutput->sendMessageNow(message);
    midiOutput->sendMessageNow(messageOff);
}

void Step::playStep()
{
    
    for (auto& n : notes) {
        playNote(n->degree);
    }
}

