#include "ModifierRegistry.h"

ModifierRegistry& ModifierRegistry::getInstance()
{
    static ModifierRegistry instance;
    return instance;
}

bool ModifierRegistry::registerModifier (ModifierDefinition def)
{
    definitions[def.type] = std::move (def);
    return true;
}

std::vector<juce::Identifier> ModifierRegistry::getRegisteredTypes() const
{
    std::vector<juce::Identifier> types;
    types.reserve (definitions.size());
    for (const auto& [type, def] : definitions)
    {
        types.push_back (type);
    }
    return types;
}

const ModifierDefinition* ModifierRegistry::getDefinition (const juce::Identifier& type) const
{
    auto it = definitions.find (type);
    if (it != definitions.end())
    {
        return &it->second;
    }
    return nullptr;
}

bool ModifierRegistry::hasModifier (const juce::Identifier& type) const
{
    return definitions.find (type) != definitions.end();
}

juce::ValueTree ModifierRegistry::createDefaultState (const juce::Identifier& type) const
{
    juce::ValueTree state (type);
    if (auto* def = getDefinition (type))
    {
        for (const auto& param : def->params)
        {
            state.setProperty (param.id, param.defaultVal, nullptr);
        }
    }
    return state;
}

// ============================================
// Compile-time registrations
// ============================================
namespace
{
static bool reg1 = ModifierRegistry::getInstance().registerModifier ({ ModifierIDs::randomTrigger,
                                                                       "Random Trigger",
                                                                       juce::Identifier ("sliderPanel"),
                                                                       { { ModifierIDs::randomTriggerProbability, "Probability", ParamWidgetType::slider, 0.5, 0.0, 1.0 } } });

static bool reg2 = ModifierRegistry::getInstance().registerModifier ({ ModifierIDs::randomOctaveShift,
                                                                       "Random Octave",
                                                                       juce::Identifier ("sliderPanel"),
                                                                       { { ModifierIDs::randomOctaveShiftProbability, "Probability", ParamWidgetType::slider, 0.5, 0.0, 1.0 },
                                                                         { ModifierIDs::randomOctaveShiftRange, "Range", ParamWidgetType::slider, 2.0, 1.0, 4.0 } } });

static bool reg3 = ModifierRegistry::getInstance().registerModifier ({ ModifierIDs::randomVelocity,
                                                                       "Random Velocity",
                                                                       juce::Identifier ("sliderPanel"),
                                                                       { { ModifierIDs::randomVelocityProbability, "Probability", ParamWidgetType::slider, 0.5, 0.0, 1.0 },
                                                                         { ModifierIDs::randomVelocityRange, "Range", ParamWidgetType::slider, 0.5, 0.0, 1.0 } } });
} // namespace
