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
    // Move down one degree up pitch
    auto& n = notes[nIndex];
    n->shiftDegreeUp();
    playNote(n->getDegree());
}

void Step::selectedNoteDown(size_t nIndex) {
    // Move down one degree in pitch
    auto& n = notes[nIndex];
    n->shiftDegreeDown();
    playNote(n->getDegree());
}

void Step::toggleMute()
{
    if (muteMode == MuteMode::muted)
        muteMode = MuteMode::unmuted;

    if (muteMode == MuteMode::unmuted)
        muteMode = MuteMode::muted;
}

size_t Step::addNote(std::function<bool(const Note&)> callback)
{
    auto note = std::make_unique<Note>(callback);
    notes.emplace_back(std::move(note));
    // Tell Step component to update
    sendChangeMessage();
    return notes.size() - 1;
}

size_t Step::removeNote(size_t noteIndex)
{
    size_t newNoteIndex = noteIndex;

    if (notes.size() > 1) {
        juce::Logger::writeToLog("size is: " + juce::String(notes.size()));

        // If we are looking at note index 0, remove from the end;
        if (noteIndex == 0) {
            notes.erase(notes.begin() + 1);
            newNoteIndex = noteIndex;
        }

        // Remove the selected note if it is not note 0
        else {
            notes.erase(notes.begin() + noteIndex);
            newNoteIndex = noteIndex - 1;
        }

        // Tell Step component to update
        sendChangeMessage();
    }

    return newNoteIndex;
}

void Step::playNote(int degree)
{
    auto message = juce::MidiMessage::noteOn (1, 64 + degree, (juce::uint8) 100);
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
        playNote(n->getDegree());
    }
}

