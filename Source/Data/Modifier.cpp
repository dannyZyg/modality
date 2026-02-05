#include "Modifier.h"
#include "ModifierRegistry.h"

Modifier::Modifier (const juce::Identifier& type)
{
    state = ModifierRegistry::getInstance().createDefaultState (type);
}

Modifier::Modifier (juce::ValueTree existingState)
    : state (std::move (existingState))
{
}

juce::Identifier Modifier::getType() const
{
    return state.getType();
}

juce::var Modifier::getValue (const juce::Identifier& paramId) const
{
    return state.getProperty (paramId, 0.0);
}

void Modifier::setValue (const juce::Identifier& paramId, const juce::var& value)
{
    state.setProperty (paramId, value, nullptr);
}

juce::ValueTree& Modifier::getState()
{
    return state;
}

const juce::ValueTree& Modifier::getState() const
{
    return state;
}

bool Modifier::operator< (const Modifier& other) const
{
    return getType().toString() < other.getType().toString();
}
