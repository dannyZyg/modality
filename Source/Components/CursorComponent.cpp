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

    // Yank flash: compute a 0->1 decay factor (1 = full flash, 0 = no flash)
    float yankBlend = 0.0f;
    if (yankFlashStartMs >= 0.0)
    {
        double elapsed = juce::Time::getMillisecondCounterHiRes() - yankFlashStartMs;
        if (elapsed < yankFlashDurationMs)
            yankBlend = static_cast<float> (1.0 - (elapsed / yankFlashDurationMs));
        else
        {
            yankFlashStartMs = -1.0;
        }
    }

    juce::Colour flashFill    = AppColours::yankFlashFill.withMultipliedAlpha (yankBlend);
    juce::Colour flashOutline = AppColours::yankFlashOutline.withMultipliedAlpha (yankBlend);

    // Get rectangle for cursor
    auto rect = CoordinateUtils::getRectAtPoint (cursor, width, height, cursor.getCurrentTimeline(), cursor.getCurrentScale());

    if (cursor.isNormalMode())
    {
        if (yankBlend > 0.0f)
        {
            g.setColour (flashFill);
            g.fillRect (rect);
        }
        g.setColour (yankBlend > 0.0f ? flashOutline : AppColours::getCursorColour (cursor.getMode()));
        g.drawRect (rect, 3.0f);
    }
    else if (cursor.isInsertMode())
    {
        g.setColour (yankBlend > 0.0f ? flashFill : blink);
        g.fillRect (rect);
        if (yankBlend > 0.0f)
        {
            g.setColour (flashOutline);
            g.drawRect (rect, 3.0f);
        }
    }

    if (cursor.isVisualLineMode() || cursor.isVisualBlockMode())
    {
        g.setColour (yankBlend > 0.0f ? flashFill : AppColours::getSelectionColour (cursor.getMode()));

        for (const auto& pos : cursor.getVisualSelectionPositions())
        {
            auto selection = CoordinateUtils::getRectAtPosition (pos, width, height, cursor.getCurrentTimeline(), cursor.getCurrentScale());
            g.fillRect (selection);
        }

        g.setColour (yankBlend > 0.0f ? flashOutline : AppColours::getCursorColour (cursor.getMode()));
        g.drawRect (rect, 2.0f);
    }
}

void CursorComponent::triggerYankFlash()
{
    yankFlashStartMs = juce::Time::getMillisecondCounterHiRes();
}

void CursorComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
}
