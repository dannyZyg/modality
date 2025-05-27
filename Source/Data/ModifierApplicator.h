// ModifierApplicator.h
#pragma once
#include "Data/Modifier.h"
#include "Data/Note.h"
#include <JuceHeader.h>
#include <functional>
#include <map>
#include <optional>

using ModifierCallback = std::function<std::optional<MidiNote> (const Modifier&, MidiNote)>;

class ModifierApplicator
{
public:
    static ModifierApplicator& getInstance()
    {
        static ModifierApplicator instance;
        return instance;
    }

    // Register callbacks for different modifier types
    void registerCallback (ModifierType type, ModifierCallback callback)
    {
        callbacks[type] = std::move (callback);
    }

    // Apply a single modifier to a note
    std::optional<MidiNote> applyModifier (const Modifier& mod, MidiNote note) const
    {
        auto it = callbacks.find (mod.getType());
        if (it != callbacks.end())
        {
            return it->second (mod, std::move (note));
        }
        return note; // Return unmodified note if no callback found
    }

    // Apply multiple modifiers in sequence
    std::optional<MidiNote> applyModifiers (const std::set<Modifier>& mods, MidiNote note) const
    {
        std::optional<MidiNote> current = std::move (note);
        for (const auto& mod : mods)
        {
            if (! current)
                return std::nullopt;
            current = applyModifier (mod, std::move (*current));
        }
        return current;
    }

private:
    ModifierApplicator() {}

    std::map<ModifierType, ModifierCallback> callbacks;
};
