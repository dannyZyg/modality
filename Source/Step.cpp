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
    addNote();
}

Step::~Step()
{
}

void Step::stepUp(size_t nIndex) {
    --notes[nIndex]->degree;
    playNote();
}  // Move up the y axis

void Step::stepDown(size_t nIndex) {
    ++notes[nIndex]->degree;
    playNote();
}  // Move down the y axis

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
    notes.emplace_back(std::make_unique<Note>(stepValue + 3));
    std::cout << "adding note" << std::endl;
}

void Step::removeNote(int noteIndex)
{
    if (noteIndex >= 0 && noteIndex < notes.size()) {
        // Remove the element safely
        std::cout << "removing note" << std::endl;
        notes.erase(notes.begin() + noteIndex);
    } else {
        std::cerr << "Index out of range!" << std::endl;
    }
}

void Step::playNote()
{
    auto message = juce::MidiMessage::noteOn (1, 64 - stepValue, (juce::uint8) 100);
    auto messageOff = juce::MidiMessage::noteOff (message.getChannel(), message.getNoteNumber());
    messageOff.setTimeStamp (message.getTimeStamp() + 0.1);
    
    auto midiOutputs = juce::MidiOutput::getAvailableDevices();
    auto midiOutput = juce::MidiOutput::openDevice(midiOutputs[0].identifier);
    juce::StringArray midiOutputNames;
    
    
    midiOutput->sendMessageNow(message);
    midiOutput->sendMessageNow(messageOff);

    //for (int i = 0; i < midiOutputs.size(); i++) {
    //    if (i == 0) {
    //        juce::Logger::writeToLog(midiOutputs[i].name);
    //        auto midiOutput = juce::MidiOutput::openDevice(midiOutputs[i].identifier);
    //        midiOutput->sendMessageNow(message);
    //        midiOutput->sendMessageNow(messageOff);
    //        juce::Logger::writeToLog("sending message");
    //    }
    //}
}

