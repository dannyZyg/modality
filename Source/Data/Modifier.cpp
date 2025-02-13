#include "Modifier.h"

Modifier::Modifier(ModifierType t) : type(t) {
    // Initialize with default values from MODIFIER_PARAMETERS
    auto it = MODIFIER_PARAMETERS.find(type);
    if (it != MODIFIER_PARAMETERS.end()) {
        for (const auto& param : it->second) {
            map[param.name] = param.defaultValue;
        }
    }
}

std::vector<std::string> Modifier::getParameterNames() const {
    std::vector<std::string> names;
    auto it = MODIFIER_PARAMETERS.find(type);
    if (it != MODIFIER_PARAMETERS.end()) {
        for (const auto& param : it->second) {
            names.push_back(param.name);
        }
    }
    return names;
}

bool Modifier::hasParameter(const std::string& key) const {
    auto it = MODIFIER_PARAMETERS.find(type);
    if (it != MODIFIER_PARAMETERS.end()) {
        return std::any_of(it->second.begin(), it->second.end(),
            [&key](const ParameterInfo& info) { return info.name == key; });
    }
    return false;
}

void Modifier::setModifierValue(const std::string& key, std::any v) {
    if (hasParameter(key)) {
        map[key] = std::move(v);
    } else {
        throw std::invalid_argument("Invalid parameter name: " + key);
    }
}

const ModifierType Modifier::getType() const { return type; }

std::any Modifier::getModifierValue(const std::string k) const
{
    return map.at(k);
}
