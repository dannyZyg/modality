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
            std::visit ([&state] (const auto& p)
                        {
                using T = std::decay_t<decltype (p)>;
                if constexpr (std::is_same_v<T, DualValueParamDefinition>)
                {
                    // id holds the Min key; derive the Max key by replacing "Min" suffix with "Max"
                    juce::Identifier maxId (p.id.toString().replace ("Min", "Max"));
                    state.setProperty (p.id, p.defaultMin, nullptr);
                    state.setProperty (maxId, p.defaultMax, nullptr);
                }
                else
                {
                    state.setProperty (p.id, p.defaultVal, nullptr);
                } },
                        param);
        }
    }
    return state;
}

// ============================================
// Compile-time registrations
// ============================================
namespace
{
static bool reg0 = ModifierRegistry::getInstance().registerModifier ({ .type = ModifierIDs::RandomPitchVariation,
                                                                       .displayName = "Pitch Variation",
                                                                       .description = "Randomly shifts the note up or down by a number of scale degrees. Range sets the maximum shift in either direction; the actual shift is chosen randomly each time.",
                                                                       .navShortcutDescription = "p",
                                                                       .componentType = juce::Identifier ("sliderPanel"),
                                                                       .params = {
                                                                           SingleValueParamDefinition { .id = ModifierIDs::RandomPitchVariationProbability,
                                                                                                        .displayName = "Probability",
                                                                                                        .widgetType = ParamWidgetType::slider,
                                                                                                        .defaultVal = 0.5,
                                                                                                        .min = 0.0,
                                                                                                        .max = 1.0,
                                                                                                        .interval = 0.01 },
                                                                           DualValueParamDefinition { .id = ModifierIDs::RandomPitchVariationRangeMin,
                                                                                                      .displayName = "Range (degrees)",
                                                                                                      .widgetType = ParamWidgetType::rangeSlider,
                                                                                                      .defaultMin = -3.0,
                                                                                                      .defaultMax = 3.0,
                                                                                                      .min = -12.0,
                                                                                                      .max = 12.0,
                                                                                                      .interval = 1.0 },
                                                                       } });

static bool reg1 = ModifierRegistry::getInstance().registerModifier ({ .type = ModifierIDs::RandomTrigger,
                                                                       .displayName = "Trigger Probability",
                                                                       .description = "Randomly suppresses notes. At probability 1.0, notes always play; lower values increase the chance a note is silenced.",
                                                                       .navShortcutDescription = "r",
                                                                       .componentType = juce::Identifier ("sliderPanel"),
                                                                       .params = {
                                                                           SingleValueParamDefinition { .id = ModifierIDs::RandomTriggerProbability,
                                                                                                        .displayName = "Probability",
                                                                                                        .widgetType = ParamWidgetType::slider,
                                                                                                        .defaultVal = 1.0,
                                                                                                        .min = 0.0,
                                                                                                        .max = 1.0,
                                                                                                        .interval = 0.01 },
                                                                       } });

static bool reg2 = ModifierRegistry::getInstance().registerModifier ({ .type = ModifierIDs::RandomOctaveShift,
                                                                       .displayName = "Random Octave",
                                                                       .description = "Randomly shifts the note up or down by whole octaves. Probability controls how often the shift applies; Range sets the octave boundary.",
                                                                       .navShortcutDescription = "o",
                                                                       .componentType = juce::Identifier ("sliderPanel"),
                                                                       .params = {
                                                                           SingleValueParamDefinition { .id = ModifierIDs::RandomOctaveShiftProbability,
                                                                                                        .displayName = "Probability",
                                                                                                        .widgetType = ParamWidgetType::slider,
                                                                                                        .defaultVal = 0.5,
                                                                                                        .min = 0.0,
                                                                                                        .max = 1.0,
                                                                                                        .interval = 0.01 },
                                                                           DualValueParamDefinition { .id = ModifierIDs::RandomOctaveShiftRangeMin,
                                                                                                      .displayName = "Range",
                                                                                                      .widgetType = ParamWidgetType::rangeSlider,
                                                                                                      .defaultMin = -1.0,
                                                                                                      .defaultMax = 1.0,
                                                                                                      .min = -4.0,
                                                                                                      .max = 4.0,
                                                                                                      .interval = 1.0 },
                                                                       } });

static bool reg3 = ModifierRegistry::getInstance().registerModifier ({ .type = ModifierIDs::RandomVelocity,
                                                                       .displayName = "Random Velocity Deviation",
                                                                       .description = "Randomly replaces the note velocity with a value within the configured range. Probability controls how often the deviation applies.",
                                                                       .navShortcutDescription = "v",
                                                                       .componentType = juce::Identifier ("sliderPanel"),
                                                                       .params = {
                                                                           SingleValueParamDefinition { .id = ModifierIDs::RandomVelocityProbability,
                                                                                                        .displayName = "Probability of Deviation",
                                                                                                        .widgetType = ParamWidgetType::slider,
                                                                                                        .defaultVal = 0.5,
                                                                                                        .min = 0.0,
                                                                                                        .max = 1.0,
                                                                                                        .interval = 0.01 },
                                                                           DualValueParamDefinition { .id = ModifierIDs::RandomVelocityRangeMin,
                                                                                                      .displayName = "Velocity Range",
                                                                                                      .widgetType = ParamWidgetType::rangeSlider,
                                                                                                      .defaultMin = 64.0,
                                                                                                      .defaultMax = 127.0,
                                                                                                      .min = 0.0,
                                                                                                      .max = 127.0,
                                                                                                      .interval = 1.0 },
                                                                       } });
} // namespace
