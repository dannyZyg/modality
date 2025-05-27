#pragma once

#include "Components/Widgets/ISelectableWidget.h"
#include "Components/Widgets/SliderWidgetComponent.h"
#include "juce_core/juce_core.h"
#include <JuceHeader.h>
#include <memory>

enum class ModifierType
{
    randomTrigger,
    randomVelocity,
    randomOctaveShift
};

enum class ParamWidgetType
{
    slider,
    toggle
};

struct Param
{
    ParamWidgetType type;
    double value;
    double min;
    double max;
};

static std::map<std::string, Param> randomTriggerParams = {
    { "Trigger Probability", Param { ParamWidgetType::slider, 0.5, 0.0, 1.0 } }
};

static std::map<std::string, Param> randomOctaveShiftParams = {
    { "Octave Shift Probability", Param { ParamWidgetType::slider, 0.5, 0.0, 1.0 } },
    { "Octave Range", Param { ParamWidgetType::slider, 0.5, 0.0, 1.0 } },
};

static std::map<std::string, Param> randomVelocityParams = {
    { "Velocity Probability", Param { ParamWidgetType::slider, 0.5, 0.0, 1.0 } },
    { "Velocity Range", Param { ParamWidgetType::slider, 0.5, 0.0, 1.0 } },
};

static std::map<ModifierType, std::map<std::string, Param>> modifierTypeToParams = {
    { ModifierType::randomTrigger, randomTriggerParams },
    { ModifierType::randomOctaveShift, randomOctaveShiftParams },
    { ModifierType::randomVelocity, randomVelocityParams },
};

[[maybe_unused]] static const juce::String getModifierName (ModifierType t)
{
    switch (t)
    {
        case ModifierType::randomTrigger:
            return "Random Trigger";
        case ModifierType::randomVelocity:
            return "Random Velocity";
        case ModifierType::randomOctaveShift:
            return "Random Octave Shift";
        default:
            return "Unknown";
    }
}

[[maybe_unused]] static const juce::String getParamWidgetClass (ParamWidgetType t)
{
    switch (t)
    {
        case ParamWidgetType::slider:
            return "Random Trigger";
        case ParamWidgetType::toggle:
            return "Random Octave Shift";
        default:
            return "Unknown";
    }
}

[[maybe_unused]] static const std::unique_ptr<ISelectableWidget> createParamWidget (const Param& param, const juce::String& title)
{
    switch (param.type)
    {
        case ParamWidgetType::slider:
            return std::make_unique<SliderWidgetComponent> (param.min, param.max, param.value, title);
        case ParamWidgetType::toggle:
            return nullptr;
        default:
            return nullptr;
    }
}

[[maybe_unused]] static const std::vector<std::unique_ptr<ISelectableWidget>> createParamWidgets (ModifierType type)
{
    std::vector<std::unique_ptr<ISelectableWidget>> widgets;

    auto outerMapIt = modifierTypeToParams.find (type);

    if (outerMapIt != modifierTypeToParams.end())
    {
        const auto& innerMap = outerMapIt->second;

        for (const auto& paramPair : innerMap)
        {
            auto widget = createParamWidget (paramPair.second, paramPair.first);
            if (widget != nullptr)
            {
                widgets.push_back (std::move (widget));
            }
        }
    }
    return widgets;
}

class Modifier
{
public:
    Modifier (ModifierType t);

    std::map<std::string, Param> parameters;

    // std::vector<std::string> getParameterNames() const;
    bool hasParameter (const std::string& key) const;
    void setModifierValue (const std::string& key, double v);
    double getModifierValue (const std::string key) const;

    const ModifierType getType() const;

    bool operator== (const Modifier& other) const
    {
        return type == other.type;
    }

    bool operator< (const Modifier& other) const
    {
        return type < other.type;
    }

private:
    ModifierType type;
};
