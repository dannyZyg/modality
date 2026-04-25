#include "ModifierComponentFactory.h"
#include "Components/Widgets/RangeSliderWidgetComponent.h"
#include "Components/Widgets/SliderWidgetComponent.h"
#include "Data/ModifierRegistry.h"

namespace ModifierComponentFactory
{

std::unique_ptr<ISelectableWidget> createWidget (const ParamDefinition& def, juce::Value valueBinding)
{
    switch (def.widgetType)
    {
        case ParamWidgetType::slider:
            return std::make_unique<SliderWidgetComponent> (def.displayName, valueBinding, def.min, def.max);
        case ParamWidgetType::toggle:
            // TODO: Implement toggle widget when needed
            return nullptr;
        default:
            return nullptr;
    }
}

std::unique_ptr<ISelectableWidget> createDualValueWidget (const ParamDefinition& def, juce::Value minValueBinding, juce::Value maxValueBinding)
{
    switch (def.widgetType)
    {
        case ParamWidgetType::rangeSlider:
            return std::make_unique<RangeSliderWidgetComponent> (def.displayName, minValueBinding, maxValueBinding, def.min, def.max);
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

        if (paramDef.widgetType == ParamWidgetType::rangeSlider)
        {
            // Derive min/max property keys by appending "Max" to the base id
            // Convention: paramDef.id is the Min key (e.g. RandomOctaveShiftRangeMin)
            //             Max key replaces "Min" suffix with "Max"
            juce::String baseId = paramDef.id.toString();
            juce::Identifier maxId (baseId.replace ("Min", "Max"));

            juce::Value minBinding = state.getPropertyAsValue (paramDef.id, nullptr);
            juce::Value maxBinding = state.getPropertyAsValue (maxId, nullptr);
            widget = createDualValueWidget (paramDef, minBinding, maxBinding);
        }
        else
        {
            juce::Value valueBinding = state.getPropertyAsValue (paramDef.id, nullptr);
            widget = createWidget (paramDef, valueBinding);
        }

        if (widget != nullptr)
            widgets.push_back (std::move (widget));
    }

    return widgets;
}

} // namespace ModifierComponentFactory

