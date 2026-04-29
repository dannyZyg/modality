#pragma once

#include "Data/Parameter.h"
#include "juce_core/juce_core.h"
#include <JuceHeader.h>
#include <map>
#include <variant>

// Type alias - used throughout codebase
using ModifierType = juce::Identifier;

namespace ModifierIDs
{
#define DECLARE_ID(name) inline const juce::Identifier name { #name };

// Modifier types
DECLARE_ID (RandomPitchVariation)
DECLARE_ID (RandomTrigger)
DECLARE_ID (RandomOctaveShift)
DECLARE_ID (RandomVelocity)

// Parameter IDs
DECLARE_ID (RandomPitchVariationProbability)
DECLARE_ID (RandomPitchVariationRangeMin)
DECLARE_ID (RandomPitchVariationRangeMax)
DECLARE_ID (RandomTriggerProbability)
DECLARE_ID (RandomOctaveShiftProbability)
DECLARE_ID (RandomOctaveShiftRangeMin)
DECLARE_ID (RandomOctaveShiftRangeMax)
DECLARE_ID (RandomVelocityProbability)
DECLARE_ID (RandomVelocityRangeMin)
DECLARE_ID (RandomVelocityRangeMax)

#undef DECLARE_ID

inline const std::array<juce::Identifier, 4> AllTypes = {
    RandomPitchVariation,
    RandomTrigger,
    RandomOctaveShift,
    RandomVelocity
};

} // namespace ModifierIDs

struct SingleValueParamDefinition
{
    juce::Identifier id;
    juce::String displayName;
    ParamWidgetType widgetType;
    double defaultVal;
    double min;
    double max;
    double interval;
};

struct DualValueParamDefinition
{
    juce::Identifier id;
    juce::String displayName;
    ParamWidgetType widgetType;
    double defaultMin;
    double defaultMax;
    double min;
    double max;
    double interval;
};

using ParamDefinition = std::variant<SingleValueParamDefinition, DualValueParamDefinition>;

// Thread-safe parameter snapshot for atomic parameter reads during modifier application
struct ModifierParameterSnapshot
{
    juce::Identifier type;
    std::map<juce::Identifier, juce::var> parameters;

    // Get parameter value with default fallback
    template <typename T>
    T getParam (const juce::Identifier& paramId, T defaultValue = T {}) const
    {
        auto it = parameters.find (paramId);
        if (it != parameters.end())
            return static_cast<T> (it->second);
        return defaultValue;
    }
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

    // Create thread-safe parameter snapshot for atomic reads during modifier application
    ModifierParameterSnapshot createParameterSnapshot() const;

    // For std::set ordering - compares by type only (one modifier per type per note)
    bool operator< (const Modifier& other) const;

private:
    juce::ValueTree state;
};
