// ModifierApplicator.h
#pragma once
#include "Data/Modifier.h"
#include "Data/Note.h"
#include "Data/Scale.h"
#include "juce_core/juce_core.h"
#include <algorithm>
#include <cstddef>
#include <functional>
#include <map>
#include <random>

using ModifierCallback = std::function<MidiNote (const Modifier&, MidiNote, const Scale&)>;

static std::mt19937 rng { std::random_device {}() };

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
    MidiNote applyModifier (const Modifier& mod, MidiNote note, const Scale& scale) const
    {
        auto it = callbacks.find (mod.getType());
        if (it != callbacks.end())
        {
            return it->second (mod, std::move (note), scale);
        }
        return note; // Return unmodified note if no callback found
    }

    // Apply multiple modifiers in sequence (vector variant)
    MidiNote applyModifiers (const std::vector<Modifier>& mods, MidiNote note, const Scale& scale) const
    {
        MidiNote current = std::move (note);
        for (const auto& mod : mods)
            current = applyModifier (mod, std::move (current), scale);
        return current;
    }

private:
    ModifierApplicator()
    {
        registerCallback (
            ModifierIDs::RandomPitchVariation,
            [] (const Modifier& modifier, MidiNote note, const Scale& scale) -> MidiNote
            {
                float probability = modifier.getValue (ModifierIDs::RandomPitchVariationProbability);
                std::uniform_real_distribution<float> probDist (0.0f, 1.0f);
                if (probDist (rng) > probability)
                    return note;

                auto rangeMin = modifier.getValue (ModifierIDs::RandomPitchVariationRangeMin);
                auto rangeMax = modifier.getValue (ModifierIDs::RandomPitchVariationRangeMax);
                if (rangeMin == rangeMax)
                    return note;

                std::uniform_int_distribution<int> stepDist (rangeMin, rangeMax);
                int steps = 0;
                while (steps == 0)
                    steps = stepDist (rng);

                Degree currentDegree (note.noteNumber - 64);
                auto shifted = scale.applySteps (currentDegree, steps, false);
                if (! shifted.has_value())
                    return note;

                note.noteNumber = std::clamp (static_cast<int> (64 + shifted->value), 0, 127);
                return note;
            });

        registerCallback (
            ModifierIDs::RandomPitchVariation,
            [] (const Modifier& modifier, MidiNote note, const Scale& scale) -> MidiNote
            {
                float probability = modifier.getValue (ModifierIDs::RandomPitchVariationProbability);
                std::uniform_real_distribution<float> probDist (0.0f, 1.0f);
                if (probDist (rng) > probability)
                    return note;

                auto rangeMin = modifier.getValue (ModifierIDs::RandomPitchVariationRangeMin);
                auto rangeMax = modifier.getValue (ModifierIDs::RandomPitchVariationRangeMax);
                if (rangeMin == rangeMax)
                    return note;

                std::uniform_int_distribution<int> stepDist (rangeMin, rangeMax);
                int steps = 0;
                while (steps == 0)
                    steps = stepDist (rng);

                Degree currentDegree (note.noteNumber - 64);
                auto shifted = scale.applySteps (currentDegree, steps, false);
                if (! shifted.has_value())
                    return note;

                note.noteNumber = std::clamp (static_cast<int> (64 + shifted->value), 0, 127);
                return note;
            });

        registerCallback (
            ModifierIDs::RandomTrigger,
            [] (const Modifier& modifier, MidiNote note, const Scale&) -> MidiNote
            {
                float probability = modifier.getValue (ModifierIDs::RandomTriggerProbability);
                std::uniform_real_distribution<float> dist (0.0f, 1.0f);

                if (dist (rng) > probability)
                {
                    note.isMuted = true;
                    return note;
                }
                return note;
            });

        registerCallback (
            ModifierIDs::RandomOctaveShift,
            [] (const Modifier& modifier, MidiNote note, const Scale&) -> MidiNote
            {
                float probability = modifier.getValue (ModifierIDs::RandomOctaveShiftProbability);
                std::uniform_real_distribution<float> dist (0.0f, 1.0f);
                if (dist (rng) > probability)
                    return note;
                auto rangeMin = modifier.getValue (ModifierIDs::RandomOctaveShiftRangeMin);
                auto rangeMax = modifier.getValue (ModifierIDs::RandomOctaveShiftRangeMax);
                std::uniform_int_distribution<int> octaveDist (rangeMin, rangeMax);
                int shift = octaveDist (rng) * 12;
                note.noteNumber = std::clamp (note.noteNumber + shift, 0, 127);
                return note;
            });

        registerCallback (
            ModifierIDs::RandomVelocity,
            [] (const Modifier& modifier, MidiNote note, const Scale&) -> MidiNote
            {
                float probability = modifier.getValue (ModifierIDs::RandomVelocityProbability);
                std::uniform_real_distribution<float> dist (0.0f, 1.0f);
                if (dist (rng) > probability)
                    return note;
                auto rangeMin = modifier.getValue (ModifierIDs::RandomVelocityRangeMin);
                auto rangeMax = modifier.getValue (ModifierIDs::RandomVelocityRangeMax);
                std::uniform_int_distribution<int> velDist (rangeMin, rangeMax);
                note.velocity = std::clamp (velDist (rng), 0, 127);
                return note;
            });
    }

    std::map<ModifierType, ModifierCallback> callbacks;
};
