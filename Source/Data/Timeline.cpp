#include "Timeline.h"
#include "juce_core/juce_core.h"
#include "juce_data_structures/juce_data_structures.h"
#include <utility>

TimePoint::TimePoint (double init)
{
    value = init;
}

Timeline::Timeline (double lower, double upper) : Timeline (juce::ValueTree())
{
    setLowerBound (lower);
    setUpperBound (upper);
}

Timeline::Timeline (juce::ValueTree existingState) : state (existingState.isValid() ? std::move (existingState) : juce::ValueTree (TimelineIDs::Timeline))
{
    if (! state.hasProperty (TimelineIDs::StepSize))
        setStepSize (Division::quarter, nullptr);

    if (! state.hasProperty (TimelineIDs::LowerBound))
        setLowerBound (0.0);

    if (! state.hasProperty (TimelineIDs::UpperBound))
        setUpperBound (4.0);
}

double Timeline::wrapTime (double time)
{
    double upper = getUpperBound();
    double lower = getLowerBound();
    double range = upper - lower;
    double wrapped = fmod (time - lower, range);
    if (wrapped < 0)
        wrapped += range;
    return lower + wrapped;
}

juce::ValueTree& Timeline::getState() { return state; }

double Timeline::clampValue (double newValue)
{
    return std::clamp (newValue, getLowerBound(), getUpperBound());
}

double Timeline::getLowerBound() const { return state.getProperty (TimelineIDs::LowerBound); }

void Timeline::setLowerBound (double lowerBound, juce::UndoManager* undoManager)
{
    state.setProperty (TimelineIDs::LowerBound, lowerBound, undoManager);
}

double Timeline::getUpperBound() const { return state.getProperty (TimelineIDs::UpperBound); }

void Timeline::setUpperBound (double upperBound, juce::UndoManager* undoManager)
{
    state.setProperty (TimelineIDs::UpperBound, upperBound, undoManager);
}

double Timeline::getStepSize() const { return state.getProperty (TimelineIDs::StepSize); }

void Timeline::setStepSize (double stepSize, juce::UndoManager* undoManager)
{
    state.setProperty (TimelineIDs::StepSize, stepSize, undoManager);
}

double Timeline::getSmallestStepSize() const { return Division::thirtysecond; }

TimePoint Timeline::getNextStep (const TimePoint& tp, bool shouldWrap) const
{
    return getNextStep (tp, getStepSize(), shouldWrap);
}

TimePoint Timeline::getNextStep (const TimePoint& tp, double division, bool shouldWrap) const
{
    double newVal = tp.value + division;

    if (newVal > getUpperBound() - division)
    {
        if (shouldWrap)
            return 0.0;
        else
            return getUpperBound() - division;
    }

    return TimePoint (newVal);
}

TimePoint Timeline::getPrevStep (const TimePoint& tp, bool shouldWrap) const
{
    return getPrevStep (tp, getStepSize(), shouldWrap);
}

TimePoint Timeline::getPrevStep (const TimePoint& tp, double division, bool shouldWrap) const
{
    double newVal = tp.value - division;

    if (newVal < getLowerBound())
    {
        if (shouldWrap)
            return getUpperBound() - division;
        else
            return 0.0;
    }

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

bool Timeline::isWithinBounds (const double time) const
{
    return time >= getLowerBound() && time < getUpperBound();
}
