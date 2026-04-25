#include "ModifierApplicator.h"
#include "Data/Modifier.h"
#include <algorithm>
#include <random>

namespace
{
static std::mt19937 rng { std::random_device {}() };

static bool reg1 = (ModifierApplicator::getInstance().registerCallback (
                        ModifierIDs::RandomTrigger,
                        [] (const Modifier& mod, MidiNote note) -> std::optional<MidiNote>
                        {
                            auto probability = static_cast<float> (mod.getValue (ModifierIDs::RandomTriggerProbability));
                            std::uniform_real_distribution<float> dist (0.0f, 1.0f);
                            if (dist (rng) > probability)
                                return std::nullopt;
                            return note;
                        }),
                    true);

static bool reg2 = (ModifierApplicator::getInstance().registerCallback (
                        ModifierIDs::RandomOctaveShift,
                        [] (const Modifier& mod, MidiNote note) -> std::optional<MidiNote>
                        {
                            auto probability = static_cast<float> (mod.getValue (ModifierIDs::RandomOctaveShiftProbability));
                            std::uniform_real_distribution<float> dist (0.0f, 1.0f);
                            if (dist (rng) > probability)
                                return note;
                            auto rangeMin = static_cast<int> (mod.getValue (ModifierIDs::RandomOctaveShiftRangeMin));
                            auto rangeMax = static_cast<int> (mod.getValue (ModifierIDs::RandomOctaveShiftRangeMax));
                            std::uniform_int_distribution<int> octaveDist (rangeMin, rangeMax);
                            int shift = octaveDist (rng) * 12;
                            note.noteNumber = std::clamp (note.noteNumber + shift, 0, 127);
                            return note;
                        }),
                    true);

static bool reg3 = (ModifierApplicator::getInstance().registerCallback (
                        ModifierIDs::RandomVelocity,
                        [] (const Modifier& mod, MidiNote note) -> std::optional<MidiNote>
                        {
                            auto probability = static_cast<float> (mod.getValue (ModifierIDs::RandomVelocityProbability));
                            std::uniform_real_distribution<float> dist (0.0f, 1.0f);
                            if (dist (rng) > probability)
                                return note;
                            auto rangeMin = static_cast<int> (mod.getValue (ModifierIDs::RandomVelocityRangeMin));
                            auto rangeMax = static_cast<int> (mod.getValue (ModifierIDs::RandomVelocityRangeMax));
                            std::uniform_int_distribution<int> velDist (rangeMin, rangeMax);
                            note.velocity = std::clamp (velDist (rng), 0, 127);
                            return note;
                        }),
                    true);
} // namespace
