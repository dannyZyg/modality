#pragma once
#include <JuceHeader.h>
#include "Data/Cursor.h"

namespace AppColours
{
    static const juce::Colour normalMode              = juce::Colours::pink;
    static const juce::Colour insertMode              = juce::Colours::lightgreen;
    static const juce::Colour visualBlockMode         = juce::Colours::lightblue.withLightness(0.7f);
    static const juce::Colour visualBlockModeCursor   = juce::Colours::darkcyan;
    static const juce::Colour visualLineMode          = juce::Colours::orange.withLightness(0.7f);
    static const juce::Colour visualLineModeCursor    = juce::Colours::orange;

    [[maybe_unused]] static juce::Colour getSelectionColour(Mode m)
    {
        switch(m)
        {
            case Mode::visualBlock:
                return AppColours::visualBlockMode;
                break;
            case Mode::visualLine:
                return AppColours::visualLineMode;
                break;
            default:
                return juce::Colours::blue;
                break;
        }

    }

    [[maybe_unused]] static juce::Colour getCursorColour(Mode m)
    {
        switch(m)
        {
            case Mode::normal:
                return AppColours::normalMode;
                break;
            case Mode::insert:
                return AppColours::insertMode;
                break;
            case Mode::visualBlock:
                return AppColours::visualBlockModeCursor;
                break;
            case Mode::visualLine:
                return AppColours::visualLineModeCursor;
                break;
            default:
                return juce::Colours::blue;
                break;
        }
    }
}
