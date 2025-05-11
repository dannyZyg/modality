

#pragma once

#include <JuceHeader.h>
#include <any>
#include <functional>
#include <unordered_map>

enum class ModifierType
{
    randomTrigger,
    velocity,
    octave
};

struct ParameterInfo
{
    std::string name;
    std::any defaultValue;
    // Optionally add more metadata like range, units, etc.
};

[[maybe_unused]] static const juce::String getModifierName (ModifierType t)
{
    switch (t)
    {
        case ModifierType::randomTrigger:
            return "Random Trigger";
            break;
        case ModifierType::velocity:
            return "Velocity";
            break;
        case ModifierType::octave:
            return "Octave";
            break;
        default:
            return "Unknown";
            break;
    }
}

static const std::unordered_map<ModifierType, std::vector<ParameterInfo>> MODIFIER_PARAMETERS = {
    { ModifierType::randomTrigger,
      {
          { "probability", 0.5 },
      } },
    { ModifierType::velocity,
      {
          { "min", 20 },
          { "max", 120 },
      } },
    { ModifierType::octave,
      { { "probability", 0.5 },
        { "range", 1 } } }
};

struct ModifierInfo
{
    char shortcut;
    juce::String displayName;
};

class Modifier
{
public:
    Modifier (ModifierType t);

    std::vector<std::string> getParameterNames() const;
    bool hasParameter (const std::string& key) const;
    void setModifierValue (const std::string& key, std::any v);
    std::any getModifierValue (const std::string k) const;

    const ModifierType getType() const;

    bool operator== (const Modifier& other) const
    {
        return type == other.type;
    }

    // Add a method to get the hash value
    size_t getHashValue() const
    {
        return static_cast<size_t> (type);
    }

    [[maybe_unused]] static std::map<ModifierType, ModifierInfo> getModifierOptions()
    {
        return {
            { ModifierType::randomTrigger, { 'r', "Random Trigger" } },
            { ModifierType::velocity, { 'v', "Velocity" } },
            { ModifierType::octave, { 'o', "Octave" } }
        };
    }

private:
    ModifierType type;
    std::unordered_map<std::string, std::any> map;
    friend struct std::hash<Modifier>;
};

namespace std
{
template <>
struct hash<Modifier>
{
    size_t operator() (const Modifier& mod) const noexcept
    {
        return mod.getHashValue();
    }
};
} // namespace std
