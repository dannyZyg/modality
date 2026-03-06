#pragma once

#include "Data/Parameter.h"
#include <JuceHeader.h>

// Type alias - used throughout codebase
using ModifierType = juce::Identifier;

namespace ModifierIDs
{
#define DECLARE_ID(name) inline const juce::Identifier name { #name };

// Modifier types
DECLARE_ID (RandomTrigger)
DECLARE_ID (RandomOctaveShift)
DECLARE_ID (RandomVelocity)

// Parameter IDs
DECLARE_ID (RandomTriggerProbability)
DECLARE_ID (RandomOctaveShiftRange)
DECLARE_ID (RandomOctaveShiftProbability)
DECLARE_ID (RandomVelocityProbability)
DECLARE_ID (RandomVelocityRange)

#undef DECLARE_ID
} // namespace ModifierIDs

struct ParamDefinition
{
    juce::Identifier id;
    juce::String displayName;
    ParamWidgetType widgetType;
    double defaultVal;
    double min;
    double max;
};

class Modifier
{
public:
    explicit Modifier (const juce::Identifier& type);
    explicit Modifier (juce::ValueTree existingState);

    juce::Identifier getType() const;
    juce::var getValue (const juce::Identifier& paramId) const;
    void setValue (const juce::Identifier& paramId, const juce::var& value);

    juce::ValueTree& getState();
    const juce::ValueTree& getState() const;

    // For std::set ordering - compares by type only (one modifier per type per note)
    bool operator< (const Modifier& other) const;

private:
    juce::ValueTree state;
};
