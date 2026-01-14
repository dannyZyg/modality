/*
  ==============================================================================

    Sequence.h
    Created: 13 Jan 2025 3:17:18pm
    Author:  Danny Keig

  ==============================================================================
*/

#pragma once
#include "Data/Note.h"

class Sequence
{
public:
    Sequence();
    ~Sequence();

    // === Timing Configuration ===
    float lengthBeats = 4.0f;

    /**
     * Get the length of this sequence in seconds at a specific tempo.
     *
     * @param tempo Tempo in BPM (global tempo from Transport)
     */
    double getLengthSeconds (double tempo) const;

    /**
     * Set the length in beats. This affects the timeline bounds.
     */
    void setLengthBeats (float beats);

    // === MIDI Output Configuration ===

    /**
     * MIDI channel for this sequence (1-16).
     */
    int midiChannel = 1;

    /**
     * MIDI output device identifier.
     * Empty string means use the default output device.
     */
    juce::String midiOutputId = "";

    /**
     * Whether this sequence is enabled for playback.
     */
    bool enabled = true;

    /**
     * Whether this sequence is muted (still processes but doesn't output).
     */
    bool muted = false;

    // === Accessors for MIDI config ===

    void setMidiChannel (int channel);
    int getMidiChannel() const;

    void setMidiOutputId (const juce::String& outputId);
    const juce::String& getMidiOutputId() const;

    void setEnabled (bool isEnabled);
    bool isEnabled() const;

    void setMuted (bool isMuted);
    bool isMuted() const;

    std::vector<std::unique_ptr<Note>> notes;

    std::vector<std::reference_wrapper<std::unique_ptr<Note>>> findNotes (double minTime, double maxTime, double minDegree, double maxDegree);
    void removeNotes (double minTime, double maxTime, double minDegree, double maxDegree);

    const Timeline& getTimeline() const;
    const Scale& getScale() const;
    Timeline& getTimeline();
    Scale& getScale();

    void increaseTimelineStepSize();
    void decreaseTimelineStepSize();

    TimePoint getNextTimelineStep (const TimePoint& tp);
    TimePoint getNextTimelineStep (const TimePoint& tp, double division);

    TimePoint getPrevTimelineStep (const TimePoint& tp);
    TimePoint getPrevTimelineStep (const TimePoint& tp, double division);

private:
    auto makeNotePredicate (double minTime, double maxTime, double minDegree, double maxDegree);

    Timeline timeline { 0.0, lengthBeats };
    Scale scale { "Natural Minor" };
};
