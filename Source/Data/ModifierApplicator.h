// ModifierApplicator.h
#pragma once
#include "Data/Modifier.h"
#include "Data/Note.h"
#include "Data/Scale.h"
#include <JuceHeader.h>
#include <functional>
#include <map>

using ModifierCallback = std::function<MidiNote (const Modifier&, MidiNote, const Scale&)>;
using ModifierSnapshotCallback = std::function<MidiNote (const ModifierParameterSnapshot&, MidiNote, const Scale&)>;

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

    // Register thread-safe snapshot-based callbacks for different modifier types
    void registerSnapshotCallback (ModifierType type, ModifierSnapshotCallback callback)
    {
        snapshotCallbacks[type] = std::move (callback);
    }

    // Apply a single modifier to a note
    MidiNote applyModifier (const Modifier& mod, MidiNote note, const Scale& scale) const
    {
        auto it = callbacks.find (mod.getType());
        if (it != callbacks.end())
        {
            return it->second (mod, std::move (note), scale);
        }
        return note; // Return unmodified note if no callback found
    }

    // Apply multiple modifiers in sequence. Modifiers always return a MidiNote;
    // we thread the note through each callback and return the final result.
    MidiNote applyModifiers (const std::set<Modifier>& mods, MidiNote note, const Scale& scale) const
    {
        MidiNote current = std::move (note);
        for (const auto& mod : mods)
            current = applyModifier (mod, std::move (current), scale);
        return current;
    }

    // Apply multiple modifiers in sequence (vector variant)
    MidiNote applyModifiers (const std::vector<Modifier>& mods, MidiNote note, const Scale& scale) const
    {
        MidiNote current = std::move (note);
        for (const auto& mod : mods)
            current = applyModifier (mod, std::move (current), scale);
        return current;
    }

    MidiNote applyModifiersThreadSafe (const std::vector<ModifierParameterSnapshot>& snapshots, MidiNote note, const Scale& scale) const
    {
        MidiNote current = std::move (note);
        for (const auto& snapshot : snapshots)
        {
            auto it = snapshotCallbacks.find (snapshot.type);
            if (it != snapshotCallbacks.end())
                current = it->second (snapshot, std::move (current), scale);
        }
        return current;
    }

private:
    ModifierApplicator() {}

    std::map<ModifierType, ModifierCallback> callbacks;
    std::map<ModifierType, ModifierSnapshotCallback> snapshotCallbacks;
};
