#pragma once

#include <JuceHeader.h>
#include <Data/Timeline.h>
#include <Data/Scale.h>
#include <Data/Cursor.h>

class CoordinateUtils
{
public:

    // Calculate width based on timeline with the current step size
    static float getStepWidthAtStepSize(float screenWidth, const Timeline& timeline) {
        return static_cast<float>(1.0 / timeline.sizeAtCurrentStepSize() * screenWidth);
    }

    // Calculate width based on timeline step size
    static float getStepWidthAtSmallestSize(float screenWidth, const Timeline& timeline) {
        return static_cast<float>(1.0 / timeline.size() * screenWidth);
    }

    static float getStepHeight(float screenHeight, const Scale& scale) {
        double totalDegreeRange = scale.getUpperBound() - scale.getLowerBound();
        return static_cast<float>((scale.stepSize / totalDegreeRange) * screenHeight);
    }

    // Convert musical time to screen X coordinate
    static float timeToScreenX(double timePos, double screenWidth, const Timeline& timeline) {
        return static_cast<float>((timePos - timeline.getLowerBound()) /
               (timeline.getUpperBound() - timeline.getLowerBound()) * screenWidth);
    }

    // Convert musical degree to screen Y coordinate
    static float degreeToScreenY(double degree, float screenHeight, const Scale& scale) {
        // Calculate height of each position
        double positionHeight = screenHeight / scale.size();

        // Convert degree to position index (24 to 0), where 12 maps to index 0
        double positionIndex = scale.getUpperBound() - degree;

        // Return y coordinate
        return static_cast<float>(positionIndex * positionHeight);
    }

    // Convert musical coordinates to screen coordinates
    static juce::Point<float> musicToScreen(const Note& note,
                                          float screenWidth, float screenHeight,
                                          const Timeline& timeline, const Scale& scale) {
        return {
            timeToScreenX(note.getStartTime(), screenWidth, timeline),
            degreeToScreenY(note.getDegree(), screenHeight, scale)
        };
    }

    // Convert musical coordinates to screen coordinates
    static juce::Point<float> musicToScreen(double timePos, double degree,
                                          float screenWidth, float screenHeight,
                                          const Timeline& timeline, const Scale& scale) {
        return {
            timeToScreenX(timePos, screenWidth, timeline),
            degreeToScreenY(degree, screenHeight, scale)
        };
    }

    // Convert musical coordinates to screen coordinates
    static juce::Point<float> musicToScreen(const Cursor& c,
                                          float screenWidth, float screenHeight,
                                          const Timeline& timeline, const Scale& scale) {
        return {
            timeToScreenX(c.cursorPosition.xTime.value, screenWidth, timeline),
            degreeToScreenY(c.cursorPosition.yDegree.value, screenHeight, scale)
        };
    }

    // Get rectangle dimensions at a point
    static juce::Rectangle<float> getRectAtPoint(double timePos, double degree,
                                               float screenWidth, float screenHeight,
                                               const Timeline& timeline, const Scale& scale) {
        auto point = musicToScreen(timePos, degree, screenWidth, screenHeight, timeline, scale);
        float width = getStepWidthAtStepSize(screenWidth, timeline);
        float height = getStepHeight(screenHeight, scale);

        return juce::Rectangle<float>(
            point.x,
            point.y,
            width,
            height
        );
    }

    // Get rectangle dimensions for cursor
    static juce::Rectangle<float> getRectAtPoint(const Cursor& c,
                                               float screenWidth, float screenHeight,
                                               const Timeline& timeline, const Scale& scale) {
        return getRectAtPoint(c.cursorPosition.xTime.value,
                            c.cursorPosition.yDegree.value,
                            screenWidth, screenHeight,
                            timeline, scale);
    }

    // Get rectangle dimensions for cursor
    static juce::Rectangle<float> getRectAtPosition(const Position& p,
                                               float screenWidth, float screenHeight,
                                               const Timeline& timeline, const Scale& scale) {
        return getRectAtPoint(p.xTime.value,
                            p.yDegree.value,
                            screenWidth, screenHeight,
                            timeline, scale);
    }

    // Get rectangle dimensions for cursor
    static juce::Rectangle<float> getRectAtPoint(const Note& n,
                                               float screenWidth, float screenHeight,
                                               const Timeline& timeline, const Scale& scale)
    {
        return getRectAtPoint(n.getStartTime(),
                            n.getDegree(),
                            screenWidth, screenHeight,
                            timeline, scale);
    }


    static juce::Path getTriangleAtPoint(Note n,
                                  float screenWidth, float screenHeight,
                                  const Timeline& timeline, const Scale& scale)
    {
        auto point = musicToScreen(n, screenWidth, screenHeight, timeline, scale);
        float width = getStepWidthAtSmallestSize(screenWidth, timeline);
        float height = getStepHeight(screenHeight, scale);

        juce::Path path;
        path.startNewSubPath (point);
        path.lineTo (juce::Point<float> (point.x + width, point.y + height / 2));
        path.lineTo (juce::Point<float> (point.x, point.y + height));
        path.closeSubPath();
        return path;
    }


private:
    // Prevent instantiation since this is a utility class
    CoordinateUtils() = delete;
    ~CoordinateUtils() = delete;
};
