/*
  ==============================================================================

    Transport.cpp
    Unified transport control implementation.

  ==============================================================================
*/

#include "Transport.h"

Transport::Transport()
{
    silentSource = std::make_unique<SilentPositionableSource>();
    transportSource.setSource (silentSource.get(), 0, nullptr, sampleRate);
}

Transport::~Transport()
{
    transportSource.setSource (nullptr);
}

// === Tempo ===

void Transport::setTempo (double bpm)
{
    // Clamp to valid range
    bpm = juce::jlimit (MIN_TEMPO, MAX_TEMPO, bpm);

    if (isPlaying())
    {
        // Queue the change to be applied at next loop boundary
        pendingTempo.store (bpm);
        tempoPending.store (true);
    }
    else
    {
        // Apply immediately when not playing
        tempo.store (bpm);
        pendingTempo.store (bpm);
    }
}

double Transport::getTempo() const
{
    return tempo.load();
}

bool Transport::hasPendingTempoChange() const
{
    return tempoPending.load();
}

double Transport::applyPendingTempo()
{
    if (tempoPending.load())
    {
        double newTempo = pendingTempo.load();
        tempo.store (newTempo);
        tempoPending.store (false);
        return newTempo;
    }
    return tempo.load();
}

// === Transport Control ===

void Transport::start()
{
    transportSource.start();
}

void Transport::stop()
{
    transportSource.stop();
    clearScheduledEvents();
}

bool Transport::isPlaying() const
{
    return transportSource.isPlaying();
}

double Transport::getCurrentPosition() const
{
    return transportSource.getCurrentPosition();
}

void Transport::setPosition (double positionSeconds)
{
    transportSource.setPosition (positionSeconds);
}

// === Track Scheduling (delegates to TransportEngine) ===

void Transport::setNumTracks (size_t numTracks)
{
    engine.setNumTracks (numTracks);
}

size_t Transport::getNumTracks() const
{
    return engine.getNumTracks();
}

void Transport::scheduleTrack (size_t trackIndex,
                               const std::vector<MidiNote>& notes,
                               double loopStartTime,
                               double loopLengthSeconds,
                               juce::MidiOutput* output,
                               int midiChannel)
{
    engine.scheduleTrack (trackIndex, notes, loopStartTime, loopLengthSeconds, output, midiChannel);
}

bool Transport::trackNeedsScheduling (size_t trackIndex) const
{
    return engine.trackNeedsScheduling (trackIndex, getCurrentPosition());
}

double Transport::getTrackNextLoopStartTime (size_t trackIndex) const
{
    return engine.getTrackNextLoopStartTime (trackIndex);
}

void Transport::markTrackScheduled (size_t trackIndex, double loopLengthSeconds)
{
    engine.markTrackScheduled (trackIndex, loopLengthSeconds);
}

void Transport::clearScheduledEvents()
{
    engine.clearScheduledEvents();
}

void Transport::reset()
{
    setPosition (0.0);
    engine.reset();

    // Clear any pending tempo change on reset
    tempoPending.store (false);
}

// === Audio Callback ===

void Transport::audioDeviceIOCallbackWithContext (
    [[maybe_unused]] const float* const* inputChannelData,
    [[maybe_unused]] int numInputChannels,
    float* const* outputChannelData,
    int numOutputChannels,
    int numSamples,
    [[maybe_unused]] const AudioIODeviceCallbackContext& context)
{
    // Clear output buffer
    for (int channel = 0; channel < numOutputChannels; ++channel)
    {
        if (outputChannelData[channel] != nullptr)
            std::fill_n (outputChannelData[channel], numSamples, 0.0f);
    }

    // Update transport position
    juce::AudioBuffer<float> tempBuffer (outputChannelData, numOutputChannels, numSamples);
    transportSource.getNextAudioBlock (juce::AudioSourceChannelInfo (tempBuffer));

    if (! isPlaying())
        return;

    double currentPosition = getCurrentPosition();
    double bufferDuration = static_cast<double> (numSamples) / sampleRate;

    // Process MIDI events - realtime safe, no allocations
    engine.processBlock (currentPosition, bufferDuration);
}

void Transport::audioDeviceAboutToStart (juce::AudioIODevice* device)
{
    sampleRate = device->getCurrentSampleRate();
    transportSource.prepareToPlay (512, sampleRate);
    engine.prepareToPlay (sampleRate);

    juce::Logger::writeToLog ("Transport: Audio device starting - " + device->getName() + " @ " + juce::String (sampleRate) + " Hz");
}

void Transport::audioDeviceStopped()
{
    transportSource.releaseResources();
    juce::Logger::writeToLog ("Transport: Audio device stopped");
}

// === Utility ===

double Transport::beatsToSeconds (double beats) const
{
    return (beats / getTempo()) * 60.0;
}

double Transport::secondsToBeats (double seconds) const
{
    return (seconds / 60.0) * getTempo();
}
