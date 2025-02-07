#pragma once

#include <JuceHeader.h>

class Degree {
public:
    Degree(double init);

    double value;
};

class Scale {

public:
    Scale(double lower, double upper);

    double clampValue(double newValue);

    double getLowerBound() const;
    double getUpperBound() const;

    Degree getHigher(const Degree& d);
    Degree getLower(const Degree& d);

    double getSmallestStepSize();

    double stepSize = 1.0;
    double getStepSize();
    double size();
private:
    double lowerBound;
    double upperBound;
};
