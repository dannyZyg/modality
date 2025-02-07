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
    /* auto note = std::make_unique<Note>(callback); */
    auto note = std::make_unique<Note>(0.0, 0.0, 0.25);
    notes.emplace_back(std::move(note));
    // Tell Step component to update
    sendChangeMessage();
    return notes.size() - 1;
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

