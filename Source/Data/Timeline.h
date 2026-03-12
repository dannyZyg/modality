#pragma once
#include "juce_data_structures/juce_data_structures.h"
#include <JuceHeader.h>

namespace TimelineIDs
{
#define DECLARE_ID(name) inline const juce::Identifier name { #name };
DECLARE_ID (Timeline)
DECLARE_ID (Value)
DECLARE_ID (StepSize)
DECLARE_ID (UpperBound)
DECLARE_ID (LowerBound)
#undef DECLARE_ID
} // namespace TimelineIDs

class TimePoint
{
public:
    TimePoint (double init);
    double value;
};

class Division
{
public:
    static constexpr double whole = 1.0;
    static constexpr double half = 0.5;
    static constexpr double quarter = 0.25;
    static constexpr double eighth = 0.125;
    static constexpr double sixteenth = 0.0625;
    static constexpr double thirtysecond = 0.03125;

    // Triplets
    static constexpr double quarterTriplet = quarter * 2.0 / 3.0;
    static constexpr double eighthTriplet = eighth * 2.0 / 3.0;

    static constexpr std::array<double, 6> values = {
        thirtysecond,
        sixteenth,
        eighth,
        quarter,
        half,
        whole
    };

    static constexpr double epsilon = 0.0001; // Small value for comparison

    static bool isEqual (double a, double b)
    {
        return std::abs (a - b) < epsilon;
    }

    static bool isLessOrEqual (double a, double b)
    {
        return a < b || isEqual (a, b);
    }

    static double getNextSmaller (double current)
    {
        for (size_t i = values.size() - 1; i > 0; --i)
        {
            if (isEqual (values[i], current))
            {
                return values[i - 1];
            }
        }
        return current; // Return same value if no smaller found
    }

    static double getNextLarger (double current)
    {
        for (size_t i = 0; i < values.size() - 1; ++i)
        {
            if (isEqual (values[i], current))
            {
                return values[i + 1];
            }
        }
        return current; // Return same value if no larger found
    }

    Division() = delete;
};

class Timeline
{
public:
    Timeline (double lower, double upper);
    explicit Timeline (juce::ValueTree existingState);

    double wrapTime (double time);
    double clampValue (double newValue);

    juce::ValueTree& getState();

    double getLowerBound() const;
    double getUpperBound() const;
    bool isWithinBounds (const double time) const;
    void setLowerBound (double lowerBound, juce::UndoManager* undoManager = nullptr);
    void setUpperBound (double upperBound, juce::UndoManager* undoManager = nullptr);

    TimePoint getNextStep (const TimePoint& tp, bool shouldWrap) const;
    TimePoint getNextStep (const TimePoint& tp, double division, bool shouldWrap) const;

    TimePoint getPrevStep (const TimePoint& tp, bool shouldWrap) const;
    TimePoint getPrevStep (const TimePoint& tp, double division, bool shouldWrap) const;

    double getStepSize() const;
    void setStepSize (double stepSize, juce::UndoManager* undoManager = nullptr);

    double getSmallestStepSize() const;

    void increaseStepSize();
    void decreaseStepSize();

    double convertBarPositionToSeconds (double barPosition, double tempo);
    double convertDivisionToSeconds (double division, double tempo);

    const double size() const;
    const double sizeAtCurrentStepSize() const;

private:
    juce::ValueTree state;
};
