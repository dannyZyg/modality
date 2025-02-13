#pragma once

#include <JuceHeader.h>

class ScaleDefinition {
public:
    ScaleDefinition(const std::string& n, const std::vector<double>& d)
        : name(n), degrees(d) {}

    std::string name;
    std::vector<double> degrees;
};

// Define static scale definitions
const std::map<std::string, std::vector<double>> scaleDefinitions = {
    {"Natural Minor", {0.0, 2.0, 3.0, 5.0, 7.0, 8.0, 10.0, 12.0}},
    {"Major", {0.0, 2.0, 4.0, 5.0, 7.0, 9.0, 11.0, 12.0}},
    {"Blues", {0.0, 3.0, 5.0, 6.0, 7.0, 10.0, 12.0}},
    {"Pentatonic", {0.0, 2.0, 4.0, 7.0, 9.0, 12.0}}
};

class Degree {
public:
    Degree(double init);

    double value;
};

class Scale {

public:
    Scale(const std::string& name);

    double getLowerBound() const;
    double getUpperBound() const;

    Degree getHigher(const Degree& d);
    Degree getLower(const Degree& d);

    double getSmallestStepSize() const;
    double getStepSize() const;

    double stepSize = 1.0;
    double size() const;
private:
    std::vector<double> degrees;
    std::vector<double> getDescendingDegrees() const;
    double lowerBound;
    double upperBound;
};

