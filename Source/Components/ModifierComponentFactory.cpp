#include "ModifierComponentFactory.h"
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

std::vector<std::unique_ptr<ISelectableWidget>> createWidgets (const juce::Identifier& modifierType, juce::ValueTree& state)
{
    std::vector<std::unique_ptr<ISelectableWidget>> widgets;

    const auto* definition = ModifierRegistry::getInstance().getDefinition (modifierType);
    if (definition == nullptr)
    {
        return widgets;
    }

    for (const auto& paramDef : definition->params)
    {
        // Get a juce::Value reference to the ValueTree property
        juce::Value valueBinding = state.getPropertyAsValue (paramDef.id, nullptr);
        auto widget = createWidget (paramDef, valueBinding);
        if (widget != nullptr)
        {
            widgets.push_back (std::move (widget));
        }
    }

    return widgets;
}

} // namespace ModifierComponentFactory
