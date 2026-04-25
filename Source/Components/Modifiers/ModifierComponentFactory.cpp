#include "ModifierComponentFactory.h"
#include "Components/Widgets/RangeSliderWidgetComponent.h"
#include "Components/Widgets/SliderWidgetComponent.h"
#include "Data/ModifierRegistry.h"

namespace ModifierComponentFactory
{

std::unique_ptr<ISelectableWidget> createWidget (const SingleValueParamDefinition& def, juce::Value valueBinding)
{
    switch (def.widgetType)
    {
        case ParamWidgetType::slider:
            return std::make_unique<SliderWidgetComponent> (def.displayName, valueBinding, def.min, def.max, def.interval);
        case ParamWidgetType::toggle:
            // TODO: Implement toggle widget when needed
            return nullptr;
        default:
            return nullptr;
    }
}

std::unique_ptr<ISelectableWidget> createDualValueWidget (const DualValueParamDefinition& def, juce::Value minValueBinding, juce::Value maxValueBinding)
{
    switch (def.widgetType)
    {
        case ParamWidgetType::rangeSlider:
            return std::make_unique<RangeSliderWidgetComponent> (def.displayName, minValueBinding, maxValueBinding, def.min, def.max, def.interval);
        default:
            return nullptr;
    }
}

std::vector<std::unique_ptr<ISelectableWidget>> createWidgets (const juce::Identifier& modifierType, juce::ValueTree& state)
{
    std::vector<std::unique_ptr<ISelectableWidget>> widgets;

    const auto* definition = ModifierRegistry::getInstance().getDefinition (modifierType);
    if (definition == nullptr)
        return widgets;

    for (const auto& paramDef : definition->params)
    {
        std::unique_ptr<ISelectableWidget> widget;

        std::visit ([&widget, &state] (const auto& p)
        {
            using T = std::decay_t<decltype (p)>;
            if constexpr (std::is_same_v<T, DualValueParamDefinition>)
            {
                // Convention: p.id is the Min key (e.g. RandomOctaveShiftRangeMin)
                //             Max key replaces "Min" suffix with "Max"
                juce::String baseId = p.id.toString();
                juce::Identifier maxId (baseId.replace ("Min", "Max"));

                juce::Value minBinding = state.getPropertyAsValue (p.id, nullptr);
                juce::Value maxBinding = state.getPropertyAsValue (maxId, nullptr);
                widget = createDualValueWidget (p, minBinding, maxBinding);
            }
            else
            {
                juce::Value valueBinding = state.getPropertyAsValue (p.id, nullptr);
                widget = createWidget (p, valueBinding);
            }
        }, paramDef);

        if (widget != nullptr)
            widgets.push_back (std::move (widget));
    }

    return widgets;
}

} // namespace ModifierComponentFactory

