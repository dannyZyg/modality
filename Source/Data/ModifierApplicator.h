// ModifierApplicator.h
#pragma once
#include <JuceHeader.h>
#include <functional>
#include <map>
#include <optional>
#include "Data/Modifier.h"
#include "Data/Note.h"

using ModifierCallback = std::function<std::optional<MidiNote>(const Modifier&, MidiNote)>;

class ModifierApplicator {
public:
    static ModifierApplicator& getInstance() {
        static ModifierApplicator instance;
        return instance;
    }

    // Register callbacks for different modifier types
    void registerCallback(ModifierType type, ModifierCallback callback) {
        callbacks[type] = std::move(callback);
    }

    // Apply a single modifier to a note
    std::optional<MidiNote> applyModifier(const Modifier& mod, MidiNote note) const {
        auto it = callbacks.find(mod.getType());
        if (it != callbacks.end()) {
            return it->second(mod, std::move(note));
        }
        return note; // Return unmodified note if no callback found
    }

    // Apply multiple modifiers in sequence
    std::optional<MidiNote> applyModifiers(const std::unordered_set<Modifier>& mods, MidiNote note) const {
        std::optional<MidiNote> current = std::move(note);
        for (const auto& mod : mods) {
            if (!current) return std::nullopt;
            current = applyModifier(mod, std::move(*current));
        }
        return current;
    }

private:
    ModifierApplicator() {
        initializeDefaultCallbacks();
    }

    void initializeDefaultCallbacks() {

        static std::mt19937 randomGenerator{std::random_device{}()};

        // Random trigger modifier
        registerCallback(ModifierType::randomTrigger,
            [](const Modifier& mod, MidiNote note) -> std::optional<MidiNote> {
                auto probability = std::any_cast<double>(mod.getModifierValue("probability"));
                std::bernoulli_distribution d(probability);
                if (d(randomGenerator)) {
                    return note;
                }
                return std::nullopt;
            });

        // Velocity modifier
        registerCallback(ModifierType::velocity,
            [](const Modifier& mod, MidiNote note) -> std::optional<MidiNote> {
                auto min = std::any_cast<int>(mod.getModifierValue("min"));
                auto max = std::any_cast<int>(mod.getModifierValue("max"));

                std::uniform_int_distribution<int> distribution(min, max);
                int newVelocity = distribution(randomGenerator);

                note.velocity = newVelocity;
                return note;
            });

        // Octave modifier
        registerCallback(ModifierType::octave,
            [](const Modifier& mod, MidiNote note) -> std::optional<MidiNote> {
                auto probability = std::any_cast<double>(mod.getModifierValue("probability"));
                auto range = std::any_cast<int>(mod.getModifierValue("range"));

                std::bernoulli_distribution d(probability);
                if (d(randomGenerator)) {
                    note.noteNumber += range * 12;
                    return note;
                }

                return note;
            });
    }

    std::map<ModifierType, ModifierCallback> callbacks;
};
