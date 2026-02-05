#pragma once

#include "Data/Modifier.h"
#include <JuceHeader.h>
#include <map>
#include <vector>

struct ModifierDefinition
{
    juce::Identifier type;
    juce::String displayName;
    juce::Identifier componentType; // UI component key (e.g., "sliderPanel")
    std::vector<ParamDefinition> params;
};

class ModifierRegistry
{
public:
    static ModifierRegistry& getInstance();

    // Registration (safe to call from static initializers)
    bool registerModifier (ModifierDefinition def);

    // Queries
    std::vector<juce::Identifier> getRegisteredTypes() const;
    const ModifierDefinition* getDefinition (const juce::Identifier& type) const;
    bool hasModifier (const juce::Identifier& type) const;

    // Factory - creates ValueTree with default parameter values
    juce::ValueTree createDefaultState (const juce::Identifier& type) const;

private:
    ModifierRegistry() = default;
    std::map<juce::Identifier, ModifierDefinition> definitions;
};
