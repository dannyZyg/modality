#include <JuceHeader.h>
#include "Components/CursorComponent.h"
#include "Components/CoordinateUtils.h"
#include "AppColours.h"

//==============================================================================
CursorComponent::CursorComponent(const Cursor& c)
    : cursor(c)
{

}

CursorComponent::~CursorComponent()
{
}

void CursorComponent::paint (juce::Graphics& g)
{
    float width = static_cast<float>(getWidth());
    float height = static_cast<float>(getHeight());

    double timeInSeconds = juce::Time::getMillisecondCounterHiRes() / 500.0f;
    double sineValue = std::sin(timeInSeconds * juce::MathConstants<float>::pi); // Oscillates between -1 and 1

    // Map sineValue to a 0 to 1 range
    float blendFactor = static_cast<float>((sineValue + 1.0f) * 0.5f);
    // Interpolate between black and light grey
    juce::Colour blink = AppColours::getCursorColour(cursor.getMode()).interpolatedWith(juce::Colours::lightgrey, blendFactor);

    auto x = CoordinateUtils::timeToScreenX(0.0, width, cursor.timeline);
    auto y = CoordinateUtils::degreeToScreenY(0.0, height, cursor.scale);
    auto base = juce::Rectangle<float>(x, y, width, CoordinateUtils::getStepHeight(height, cursor.scale));

    g.setColour (juce::Colours::lightgrey.withLightness(0.9f));
    g.fillRect(base);
    g.setColour (juce::Colours::black.withLightness(0.7f));
    g.drawRect(base, 1.0f);

    // Get rectangle for cursor
    auto rect = CoordinateUtils::getRectAtPoint(cursor, width, height, cursor.timeline, cursor.scale);

    if (cursor.isNormalMode()) {
        g.setColour (AppColours::getCursorColour(cursor.getMode()));
        g.drawRect(rect, 3.0f);
    } else if (cursor.isInsertMode()) {
        g.setColour(blink);
        g.fillRect(rect);
    }

    if (cursor.isVisualLineMode() || cursor.isVisualBlockMode()) {
        g.setColour(AppColours::getSelectionColour(cursor.getMode()));

        for (const auto& pos : cursor.getVisualSelectionPositions()) {
            // Get rectangle for cursor
            auto selection = CoordinateUtils::getRectAtPosition(pos, width, height, cursor.timeline, cursor.scale);
            g.fillRect(selection);
        }

        g.setColour (AppColours::getCursorColour(cursor.getMode()));
        g.drawRect(rect, 2.0f);
    }

}


void CursorComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}
