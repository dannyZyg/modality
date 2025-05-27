#include "Modifier.h"

Modifier::Modifier (ModifierType t) : type (t)
{
    auto iterator = modifierTypeToParams.find (type);

    if (iterator != modifierTypeToParams.end())
    {
        parameters = iterator->second;
    }
}

bool Modifier::hasParameter (const std::string& key) const
{
    auto it = parameters.find (key);
    if (it != parameters.end())
    {
        return true;
    }
    return false;
}

void Modifier::setModifierValue (const std::string& key, double v)
{
    if (hasParameter (key))
    {
        auto param = parameters.at (key);
        param.value = std::move (v);
    }
    else
    {
        throw std::invalid_argument ("Invalid parameter name: " + key);
    }
}

const ModifierType Modifier::getType() const { return type; }

double Modifier::getModifierValue (const std::string key) const
{
    try
    {
        return parameters.at (key).value;
    }
    catch (const std::out_of_range& e)
    {
        std::cerr << "Error: Parameter '" << key << "' not found. " << e.what() << std::endl;
        return {};
    }
}
