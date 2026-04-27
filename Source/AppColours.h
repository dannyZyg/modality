#pragma once
#include "Data/Cursor.h"
#include <JuceHeader.h>

namespace AppColours
{
static const juce::Colour normalMode = juce::Colours::pink;
static const juce::Colour insertMode = juce::Colours::lightgreen;
static const juce::Colour visualBlockMode = juce::Colours::lightblue.withLightness (0.7f);
static const juce::Colour visualBlockModeCursor = juce::Colours::darkcyan.withLightness (0.3f);
static const juce::Colour visualLineMode = juce::Colours::orange.withLightness (0.7f);
static const juce::Colour visualLineModeCursor = juce::Colours::orange;
static const juce::Colour playhead = juce::Colour (47, 128, 107);
static const juce::Colour yankFlashFill = juce::Colour (255, 240, 80);
static const juce::Colour yankFlashOutline = juce::Colour (255, 240, 80).darker (0.4f);
static const juce::Colour velocityFlash = juce::Colour (80, 220, 255);

[[maybe_unused]] static juce::Colour getSelectionColour (Mode m)
{
    switch (m)
    {
        case Mode::visualBlock:
            return AppColours::visualBlockMode;
        case Mode::visualLine:
            return AppColours::visualLineMode;
        case Mode::normal:
        case Mode::insert:
        case Mode::noteEdit:
        default:
            return juce::Colours::blue;
    }
}

[[maybe_unused]] static juce::Colour getCursorColour (Mode m)
{
    switch (m)
    {
        case Mode::normal:
            return AppColours::normalMode;
        case Mode::insert:
            return AppColours::insertMode;
        case Mode::visualBlock:
            return AppColours::visualBlockModeCursor;
        case Mode::visualLine:
            return AppColours::visualLineModeCursor;
        case Mode::noteEdit:
        default:
            return juce::Colours::blue;
    }
}
// Sequence selection component
static const juce::Colour sequenceSelected       = juce::Colours::orchid;
static const juce::Colour sequenceMutedSelected  = juce::Colours::orchid.withAlpha (0.35f);
static const juce::Colour sequenceMutedFill      = juce::Colours::darkgrey.withAlpha (0.4f);
static const juce::Colour sequenceSoloFill       = juce::Colour (0xffE8A020);
static const juce::Colour sequenceSoloFlash      = juce::Colour (0xffFFCC44);
static const juce::Colour sequenceOutline        = juce::Colours::grey;
static const juce::Colour sequenceMutedOutline   = juce::Colours::grey.withAlpha (0.4f);
static const juce::Colour sequenceTextSelected   = juce::Colours::white;
static const juce::Colour sequenceText           = juce::Colours::black;
static const juce::Colour sequenceTextMuted      = juce::Colours::grey.withAlpha (0.5f);
} // namespace AppColours
