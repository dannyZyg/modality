#include "Components/CursorComponent.h"
#include "AppColours.h"
#include "Components/CoordinateUtils.h"
#include <JuceHeader.h>

//==============================================================================
CursorComponent::CursorComponent (const Cursor& c)
    : cursor (c)
{
    setWantsKeyboardFocus (false);
}

CursorComponent::~CursorComponent()
{
}

void CursorComponent::paint (juce::Graphics& g)
{
    float width = static_cast<float> (getWidth());
    float height = static_cast<float> (getHeight());

    double timeInSeconds = juce::Time::getMillisecondCounterHiRes() / 500.0f;
    double sineValue = std::sin (timeInSeconds * juce::MathConstants<float>::pi); // Oscillates between -1 and 1

    // Map sineValue to a 0 to 1 range
    float blendFactor = static_cast<float> ((sineValue + 1.0f) * 0.5f);
    // Interpolate between black and light grey
    juce::Colour blink = AppColours::getCursorColour (cursor.getMode()).interpolatedWith (juce::Colours::lightgrey, blendFactor);

    // Get rectangle for cursor
    auto rect = CoordinateUtils::getRectAtPoint (cursor, width, height, cursor.getCurrentTimeline(), cursor.getCurrentScale());

    if (cursor.isNormalMode())
    {
        g.setColour (AppColours::getCursorColour (cursor.getMode()));
        g.drawRect (rect, 3.0f);
    }
    else if (cursor.isInsertMode())
    {
        g.setColour (blink);
        g.fillRect (rect);
    }

    if (cursor.isVisualLineMode() || cursor.isVisualBlockMode())
    {
        g.setColour (AppColours::getSelectionColour (cursor.getMode()));

        for (const auto& pos : cursor.getVisualSelectionPositions())
        {
            // Get rectangle for cursor
            auto selection = CoordinateUtils::getRectAtPosition (pos, width, height, cursor.getCurrentTimeline(), cursor.getCurrentScale());
            g.fillRect (selection);
        }

        g.setColour (AppColours::getCursorColour (cursor.getMode()));
        g.drawRect (rect, 2.0f);
    }
}

void CursorComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
}
