#include "Timeline.h"

TimePoint::TimePoint (double init)
{
    value = init;
}

Timeline::Timeline (double lower, double upper) : state ("Timeline")
{
    setStepSize (Division::quarter, nullptr);
    state.setProperty (TimelineIDs::lowerBound, lower, nullptr);
    state.setProperty (TimelineIDs::upperBound, upper, nullptr);
}

juce::ValueTree& Timeline::getState() { return state; }

double Timeline::clampValue (double newValue)
{
    return std::clamp (newValue, getLowerBound(), getUpperBound());
}

double Timeline::getLowerBound() const { return state.getProperty (TimelineIDs::lowerBound); }

void Timeline::setLowerBound (double lowerBound, juce::UndoManager* undoManager)
{
    state.setProperty (TimelineIDs::lowerBound, lowerBound, undoManager);
}

double Timeline::getUpperBound() const { return state.getProperty (TimelineIDs::upperBound); }

void Timeline::setUpperBound (double upperBound, juce::UndoManager* undoManager)
{
    state.setProperty (TimelineIDs::upperBound, upperBound, undoManager);
}

double Timeline::getStepSize() const { return state.getProperty (TimelineIDs::stepSize); }

void Timeline::setStepSize (double stepSize, juce::UndoManager* undoManager)
{
    state.setProperty (TimelineIDs::stepSize, stepSize, undoManager);
}

double Timeline::getSmallestStepSize() const { return Division::thirtysecond; }

TimePoint Timeline::getNextStep (const TimePoint& tp)
{
    return getNextStep (tp, getStepSize());
}

TimePoint Timeline::getNextStep (const TimePoint& tp, double division)
{
    double newVal = tp.value + division;

    if (newVal > getUpperBound() - division)
        return 0.0;

    return TimePoint (newVal);
}

TimePoint Timeline::getPrevStep (const TimePoint& tp)
{
    return getPrevStep (tp, getStepSize());
}

TimePoint Timeline::getPrevStep (const TimePoint& tp, double division)
{
    double newVal = tp.value - division;

    if (newVal < getLowerBound())
        return getUpperBound() - division;

    return TimePoint (newVal);
}

// Converts bar position to seconds based on tempo
double Timeline::convertBarPositionToSeconds (double barPosition, double tempo)
{
    const double MIN_IN_SECONDS = 60.0;
    double secondsPerBeat = MIN_IN_SECONDS / tempo;

    return secondsPerBeat * barPosition;
}

double Timeline::convertDivisionToSeconds (double division, double tempo)
{
    const double MIN_IN_SECONDS = 60.0;
    double secondsPerBeat = MIN_IN_SECONDS / tempo;

    // Assuming 4/4 time and quarter note = 1 beat
    return secondsPerBeat * division;
}

void Timeline::increaseStepSize()
{
    setStepSize (Division::getNextLarger (getStepSize()));
}

void Timeline::decreaseStepSize()
{
    setStepSize (Division::getNextSmaller (getStepSize()));
}

const double Timeline::size() const
{
    // The number of potential steps at the smallest possible step size
    return getUpperBound() / getSmallestStepSize();
}

const double Timeline::sizeAtCurrentStepSize() const
{
    // The number of potential steps at the current step size
    return getUpperBound() / getStepSize();
}
