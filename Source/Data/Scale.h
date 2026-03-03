#pragma once

#include "juce_data_structures/juce_data_structures.h"
#include <JuceHeader.h>

namespace ScaleIDs
{
#define DECLARE_ID(name) inline const juce::Identifier name { #name };
DECLARE_ID (name)
DECLARE_ID (degrees)
#undef DECLARE_ID
} // namespace ScaleIDs

// Define static scale definitions
const std::map<juce::String, std::vector<double>> scaleDefinitions = {
    { "Natural Minor", { 0.0, 2.0, 3.0, 5.0, 7.0, 8.0, 10.0, 12.0 } },
    { "Major", { 0.0, 2.0, 4.0, 5.0, 7.0, 9.0, 11.0, 12.0 } },
    { "Blues", { 0.0, 3.0, 5.0, 6.0, 7.0, 10.0, 12.0 } },
    { "Pentatonic", { 0.0, 2.0, 4.0, 7.0, 9.0, 12.0 } }
};

class Degree
{
public:
    Degree (double init);
    double value;
};

class Scale
{
public:
    Scale (const juce::String& name);

    double getLowerBound() const;
    double getUpperBound() const;

    const Degree getHigher (const Degree& d) const;
    const Degree getLower (const Degree& d) const;

    double getSmallestStepSize() const;

    double size() const;

    std::vector<double> getDegrees() const;
    juce::String getName() const;

    void setScale (juce::String scaleName, juce::UndoManager* undoManager = nullptr);
    juce::ValueTree& getState();

private:
    juce::ValueTree state;
    std::vector<double> getDescendingDegrees() const;

    void setDegrees (std::vector<double> degrees, juce::UndoManager* undoManager = nullptr);
    void setName (juce::String name, juce::UndoManager* undoManager = nullptr);
};
