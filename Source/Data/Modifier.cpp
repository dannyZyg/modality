
#include "Modifier.h"

Modifier::Modifier(ModifierType t) : type(t)
{
}


const ModifierType Modifier::getType() const { return type; }

void Modifier::setModifierValue(std::string k, std::any v)
{
    map[k] = v;
}

std::any Modifier::getModifierValue(std::string k)
{
    return map[k];
}
