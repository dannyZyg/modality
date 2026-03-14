#pragma once

#include "Components/Widgets/ISelectableWidget.h"
#include "Data/Modifier.h"
#include <JuceHeader.h>
#include <memory>
#include <vector>

namespace ModifierComponentFactory
{
// Creates widgets for all parameters of a modifier type, bound to a ValueTree via juce::Value
std::vector<std::unique_ptr<ISelectableWidget>> createWidgets (const juce::Identifier& modifierType, juce::ValueTree& state);

// Creates a single widget for a parameter, bound to a ValueTree property
std::unique_ptr<ISelectableWidget> createWidget (const ParamDefinition& def, juce::Value valueBinding);
} // namespace ModifierComponentFactory
