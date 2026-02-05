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
} // namespace AppColours
