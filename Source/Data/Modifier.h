

#pragma once


#include <JuceHeader.h>
#include <any>
#include <functional>
#include <unordered_map>

enum class ModifierType { randomTrigger, velocity, octave };

struct ModifierInfo {
    char shortcut;
    juce::String displayName;
};

class Modifier {


public:
    Modifier(ModifierType t);

    void setModifierValue(std::string k, std::any v);
    std::any getModifierValue(std::string k);

    const ModifierType getType() const;

    bool operator==(const Modifier& other) const {
        return type == other.type;
    }

    // Add a method to get the hash value
    size_t getHashValue() const {
        return static_cast<size_t>(type);
    }

    [[maybe_unused]] static std::map<ModifierType, ModifierInfo> getModifierOptions()
    {
        return {
            {ModifierType::randomTrigger, {'r', "Random Trigger"}},
            {ModifierType::velocity,      {'v', "Velocity"}},
            {ModifierType::octave,      {'o', "Octave"}}
        };
    }

private:
    ModifierType type;
    std::unordered_map<std::string, std::any> map;
    friend struct std::hash<Modifier>;
};

namespace std {
    template<>
    struct hash<Modifier> {
        size_t operator()(const Modifier& mod) const noexcept {
            return mod.getHashValue();
        }
    };
}
