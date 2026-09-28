#include "Transport.h"

Transport::Transport (Scheduler& s) : scheduler (s)
{
    silentSource = std::make_unique<SilentPositionableSource>();
    transportSource.setSource (silentSource.get(), 0, nullptr, sampleRate);
}

Transport::~Transport()
{
    transportSource.setSource (nullptr);
}

void Transport::start()
{
    transportSource.start();
}

void Transport::stop()
{
    transportSource.stop();
    reset();
}

bool Transport::isPlaying() const
{
    return transportSource.isPlaying();
}

double Transport::getCurrentPositionSeconds() const
{
    return transportSource.getCurrentPosition();
}

void Transport::setPosition (double positionSeconds)
{
    transportSource.setPosition (positionSeconds);
}

void Transport::reset()
{
    setPosition (0.0);
    scheduler.reset();
}

void Transport::audioDeviceIOCallbackWithContext (
    [[maybe_unused]] const float* const* inputChannelData,
    [[maybe_unused]] int numInputChannels,
    float* const* outputChannelData,
    int numOutputChannels,
    int numSamples,
    [[maybe_unused]] const juce::AudioIODeviceCallbackContext& context)
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

    double currentPosition = getCurrentPositionSeconds();
    double bufferDuration = static_cast<double> (numSamples) / sampleRate;

    // Process MIDI events - realtime safe, no allocations
    scheduler.processBlock (currentPosition, bufferDuration);
}

void Transport::audioDeviceAboutToStart (juce::AudioIODevice* device)
{
    sampleRate = device->getCurrentSampleRate();
    transportSource.prepareToPlay (512, sampleRate);
    scheduler.prepareToPlay (sampleRate);

    juce::Logger::writeToLog ("Transport: Audio device starting - " + device->getName() + " @ " + juce::String (sampleRate) + " Hz");
}

void Transport::audioDeviceStopped()
{
    transportSource.releaseResources();
    juce::Logger::writeToLog ("Transport: Audio device stopped");
}
