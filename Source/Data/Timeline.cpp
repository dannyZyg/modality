#include "Timeline.h"


TimePoint::TimePoint(double init)
{
    value = init;
}

Timeline::Timeline(double lower, double upper) : lowerBound(lower), upperBound(upper) { }

double Timeline::clampValue(double newValue)
{
    return std::clamp(newValue, lowerBound, upperBound);
}

double Timeline::getLowerBound() const { return lowerBound; }
double Timeline::getUpperBound() const { return upperBound; }

double Timeline::getStepSize() const { return stepSize; }

double Timeline::getSmallestStepSize() const { return Division::thirtysecond; }

TimePoint Timeline::getNextStep(const TimePoint& tp)
{
    return getNextStep(tp, stepSize);
}

TimePoint Timeline::getNextStep(const TimePoint& tp, double division)
{
    double newVal = tp.value + division;

    if (newVal > getUpperBound() - division)
        return 0.0;

    return TimePoint(newVal);
}

TimePoint Timeline::getPrevStep(const TimePoint& tp)
{
    return getPrevStep(tp, stepSize);
}

TimePoint Timeline::getPrevStep(const TimePoint& tp, double division)
{
    double newVal = tp.value - division;

    if (newVal < getLowerBound())
        return getUpperBound() - division;

    return TimePoint(newVal);
}

// Converts bar position to seconds based on tempo
double Timeline::convertBarPositionToSeconds(double barPosition, double tempo)
{
    const double MIN_IN_SECONDS = 60.0;
    double secondsPerBeat = MIN_IN_SECONDS / tempo;

    return secondsPerBeat * barPosition;
}

void Timeline::increaseStepSize()
{
    stepSize = Division::getNextLarger(stepSize);
}

void Timeline::decreaseStepSize()
{
    stepSize = Division::getNextSmaller(stepSize);
}

const double Timeline::size() const
{
    // The number of potential steps at the smallest possible step size
    return getUpperBound() / getSmallestStepSize();
}

const double Timeline::sizeAtCurrentStepSize() const
{
    // The number of potential steps at the current step size
    return getUpperBound() / stepSize;
}
