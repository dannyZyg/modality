#include "Scale.h"

Degree::Degree(double init)
{
    value = init;
}

Scale::Scale(double lower, double upper) : lowerBound(lower), upperBound(upper) { }

double Scale::clampValue(double newValue)
{
    return std::clamp(newValue, lowerBound, upperBound);
}

double Scale::getLowerBound() const { return lowerBound; }
double Scale::getUpperBound() const { return upperBound; }


Degree Scale::getHigher(const Degree& d)
{
    return Degree(clampValue(d.value + 1.0));
}

Degree Scale::getLower(const Degree& d)
{
    return Degree(clampValue(d.value - 1.0));
}

double Scale::getSmallestStepSize()
{
    return stepSize;
}

double Scale::getStepSize() const
{
    return stepSize;
}

double Scale::size() const
{
    return (getUpperBound() - getLowerBound()) + 1;
}
