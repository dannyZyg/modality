#include <JuceHeader.h>
#include "CursorComponent.h"
#include "Components/CoordinateUtils.h"

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
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */



    float timeInSeconds = juce::Time::getMillisecondCounterHiRes() / 500.0f;
    float sineValue = std::sin(timeInSeconds * juce::MathConstants<float>::pi); // Oscillates between -1 and 1

    // Map sineValue to a 0 to 1 range
    float blendFactor = (sineValue + 1.0f) * 0.5f;
    // Interpolate between black and light grey
    juce::Colour blink = juce::Colours::pink.interpolatedWith(juce::Colours::lightgrey, blendFactor);



    /* g.drawRect (getLocalBounds(), 1);   // draw an outline around the component */

    // Scenario 3: Converting both time and degree to a screen point
    juce::Point<float> screenPos = CoordinateUtils::musicToScreen(
        cursor,
        getWidth(),
        getHeight(),
        cursor.timeline,
        cursor.scale
    );



    /* g.fillRect(screenPos.x,    // x position (centered) */
    /*            screenPos.y,     // y position (centered) */
    /*            10.0,                     // width */
    /*            10.0); */

    // Get rectangle for cursor
    auto rect = CoordinateUtils::getRectAtPoint(cursor,
                                              getWidth(),
                                              getHeight(),
                                              cursor.timeline,
                                              cursor.scale);



    if (cursor.isNormalMode()) {
        g.setColour (juce::Colours::pink);
        g.drawRect(rect, 2.0f);
    } else if (cursor.isInsertMode()) {
        g.setColour(blink);
        g.fillRect(rect);
    }


    if (cursor.isVisualLineMode() || cursor.isVisualBlockMode()) {
        g.setColour(juce::Colours::lightblue);

        for (const auto& pos : cursor.getVisualSelectionPositions()) {
            // Get rectangle for cursor
            auto rect = CoordinateUtils::getRectAtPosition(pos,
                                                      getWidth(),
                                                      getHeight(),
                                                      cursor.timeline,
                                                      cursor.scale);
            g.fillRect(rect);
        }

        g.setColour (juce::Colours::lightcoral);
        g.drawRect(rect, 2.0f);
    }

}


void CursorComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}
