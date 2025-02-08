#pragma once
#include <JuceHeader.h>

class TimePoint {
public:
    TimePoint(double init);
    double value;
};


class Division {
public:
    static constexpr double whole = 1.0;
    static constexpr double half = 0.5;
    static constexpr double quarter = 0.25;
    static constexpr double eighth = 0.125;
    static constexpr double sixteenth = 0.0625;
    static constexpr double thirtysecond = 0.03125;

    // Triplets
    static constexpr double quarterTriplet = quarter * 2.0/3.0;
    static constexpr double eighthTriplet = eighth * 2.0/3.0;

    static constexpr std::array<double, 6> values = {
        thirtysecond, sixteenth, eighth, quarter, half, whole
    };

    static constexpr double epsilon = 0.0001; // Small value for comparison

    static bool isEqual(double a, double b) {
        return std::abs(a - b) < epsilon;
    }

    static bool isLessOrEqual(double a, double b) {
        return a < b || isEqual(a, b);
    }

    static double getNextSmaller(double current) {
        for (size_t i = values.size() - 1; i > 0; --i) {
            if (isEqual(values[i], current)) {
                return values[i-1];
            }
        }
        return current; // Return same value if no smaller found
    }

    static double getNextLarger(double current) {
        for (size_t i = 0; i < values.size() - 1; ++i) {
            if (isEqual(values[i], current)) {
                return values[i+1];
            }
        }
        return current; // Return same value if no larger found
    }

    Division() = delete;
};

class Timeline {

public:
    Timeline(double lower, double upper);


    double clampValue(double newValue);

    double getLowerBound() const;
    double getUpperBound() const;

    TimePoint getNextStep(const TimePoint& tp);
    TimePoint getNextStep(const TimePoint& tp, double division);

    TimePoint getPrevStep(const TimePoint& tp);
    TimePoint getPrevStep(const TimePoint& tp, double division);

    double getStepSize() const;
    double getSmallestStepSize() const;

    void increaseStepSize();
    void decreaseStepSize();

    // Converts bar position to seconds based on tempo
    double convertBarPositionToSeconds(double barPosition, double tempo);

    const double size() const;
    const double sizeAtCurrentStepSize() const;
private:
    double stepSize = Division::whole;
    double value;
    double lowerBound;
    double upperBound;
};
