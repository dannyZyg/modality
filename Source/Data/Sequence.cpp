/*
  ==============================================================================

    Sequence.cpp
    Created: 13 Jan 2025 3:17:18pm
    Author:  Danny Keig

  ==============================================================================
*/

#include "Sequence.h"

Sequence::Sequence()
{
}

Sequence::~Sequence()
{
}

double Sequence::getLengthSeconds (double tempo) const
{
    return (static_cast<double> (lengthBeats) / tempo) * 60.0;
}

void Sequence::setLengthBeats (float beats)
{
    lengthBeats = beats;
    // Recreate timeline with new bounds
    timeline = Timeline (0.0, static_cast<double> (lengthBeats));
}

// === MIDI Configuration Accessors ===

void Sequence::setMidiChannel (int channel)
{
    midiChannel = juce::jlimit (1, 16, channel);
}

int Sequence::getMidiChannel() const
{
    return midiChannel;
}

void Sequence::setMidiOutputId (const juce::String& outputId)
{
    midiOutputId = outputId;
}

const juce::String& Sequence::getMidiOutputId() const
{
    return midiOutputId;
}

void Sequence::setEnabled (bool isEnabled)
{
    enabled = isEnabled;
}

bool Sequence::isEnabled() const
{
    return enabled;
}

void Sequence::setMuted (bool isMuted)
{
    muted = isMuted;
}

bool Sequence::isMuted() const
{
    return muted;
}

// Create a reusable predicate
auto Sequence::makeNotePredicate (double minTime, double maxTime, double minDegree, double maxDegree)
{
    return [=] (const auto& note)
    {
        return note->getStartTime() >= minTime && note->getStartTime() < maxTime && note->getDegree() >= minDegree && note->getDegree() <= maxDegree;
    };
}

std::vector<std::reference_wrapper<std::unique_ptr<Note>>> Sequence::findNotes (
    double minTime,
    double maxTime,
    double minDegree,
    double maxDegree)
{
    std::vector<std::reference_wrapper<std::unique_ptr<Note>>> result;
    auto predicate = makeNotePredicate (minTime, maxTime, minDegree, maxDegree);

    for (auto& note : notes)
    {
        if (predicate (note))
        {
            result.push_back (std::ref (note));
        }
    }

    return result;
}

void Sequence::removeNotes (
    double minTime,
    double maxTime,
    double minDegree,
    double maxDegree)
{
    auto predicate = makeNotePredicate (minTime, maxTime, minDegree, maxDegree);
    notes.erase (
        std::remove_if (notes.begin(), notes.end(), predicate),
        notes.end());
}

const Timeline& Sequence::getTimeline() const { return timeline; }

Timeline& Sequence::getTimeline() { return timeline; }

const Scale& Sequence::getScale() const { return scale; }

Scale& Sequence::getScale() { return scale; }

void Sequence::increaseTimelineStepSize()
{
    timeline.increaseStepSize();
}

void Sequence::decreaseTimelineStepSize()
{
    timeline.decreaseStepSize();
}

TimePoint Sequence::getNextTimelineStep (const TimePoint& tp)
{
    return timeline.getNextStep (tp);
}

TimePoint Sequence::getNextTimelineStep (const TimePoint& tp, double division)
{
    return timeline.getNextStep (tp, division);
}

TimePoint Sequence::getPrevTimelineStep (const TimePoint& tp)
{
    return timeline.getPrevStep (tp);
}

TimePoint Sequence::getPrevTimelineStep (const TimePoint& tp, double division)
{
    return timeline.getPrevStep (tp, division);
}
