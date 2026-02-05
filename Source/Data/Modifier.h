#pragma once

#include "Data/Parameter.h"
#include <JuceHeader.h>

// Type alias - used throughout codebase
using ModifierType = juce::Identifier;

namespace ModifierIDs
{
#define DECLARE_ID(name) inline const juce::Identifier name { #name };

// Modifier types
DECLARE_ID (randomTrigger)
DECLARE_ID (randomOctaveShift)
DECLARE_ID (randomVelocity)

// Parameter IDs
DECLARE_ID (randomTriggerProbability)
DECLARE_ID (randomOctaveShiftRange)
DECLARE_ID (randomOctaveShiftProbability)
DECLARE_ID (randomVelocityProbability)
DECLARE_ID (randomVelocityRange)

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
