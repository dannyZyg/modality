#include "ModifierApplicator.h"
#include "Data/Modifier.h"
#include "Data/Scale.h"
#include <algorithm>
#include <random>

namespace
{
static std::mt19937 rng { std::random_device {}() };

static bool reg0 = (ModifierApplicator::getInstance().registerSnapshotCallback (
                        ModifierIDs::RandomPitchVariation,
                        [] (const ModifierParameterSnapshot& snapshot, MidiNote note, const Scale& scale) -> MidiNote
                        {
                            auto probability = snapshot.getParam<float> (ModifierIDs::RandomPitchVariationProbability, 0.0f);
                            std::uniform_real_distribution<float> probDist (0.0f, 1.0f);
                            if (probDist (rng) > probability)
                                return note;

                            auto rangeMin = snapshot.getParam<int> (ModifierIDs::RandomPitchVariationRangeMin, 0);
                            auto rangeMax = snapshot.getParam<int> (ModifierIDs::RandomPitchVariationRangeMax, 0);
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
                        }),
                    true);

static bool reg1 = (ModifierApplicator::getInstance().registerSnapshotCallback (
                        ModifierIDs::RandomTrigger,
                        [] (const ModifierParameterSnapshot& snapshot, MidiNote note, const Scale&) -> MidiNote
                        {
                            auto probability = snapshot.getParam<float> (ModifierIDs::RandomTriggerProbability, 0.0f);
                            std::uniform_real_distribution<float> dist (0.0f, 1.0f);

                            if (dist (rng) > probability)
                            {
                                note.isMuted = true;
                                return note;
                            }
                            return note;
                        }),
                    true);

static bool reg2 = (ModifierApplicator::getInstance().registerSnapshotCallback (
                        ModifierIDs::RandomOctaveShift,
                        [] (const ModifierParameterSnapshot& snapshot, MidiNote note, const Scale&) -> MidiNote
                        {
                            auto probability = snapshot.getParam<float> (ModifierIDs::RandomOctaveShiftProbability, 0.0f);
                            std::uniform_real_distribution<float> dist (0.0f, 1.0f);
                            if (dist (rng) > probability)
                                return note;
                            auto rangeMin = snapshot.getParam<int> (ModifierIDs::RandomOctaveShiftRangeMin, 0);
                            auto rangeMax = snapshot.getParam<int> (ModifierIDs::RandomOctaveShiftRangeMax, 0);
                            std::uniform_int_distribution<int> octaveDist (rangeMin, rangeMax);
                            int shift = octaveDist (rng) * 12;
                            note.noteNumber = std::clamp (note.noteNumber + shift, 0, 127);
                            return note;
                        }),
                    true);

static bool reg3 = (ModifierApplicator::getInstance().registerSnapshotCallback (
                        ModifierIDs::RandomVelocity,
                        [] (const ModifierParameterSnapshot& snapshot, MidiNote note, const Scale&) -> MidiNote
                        {
                            auto probability = snapshot.getParam<float> (ModifierIDs::RandomVelocityProbability, 0.0f);
                            std::uniform_real_distribution<float> dist (0.0f, 1.0f);
                            if (dist (rng) > probability)
                                return note;
                            auto rangeMin = snapshot.getParam<int> (ModifierIDs::RandomVelocityRangeMin, 0);
                            auto rangeMax = snapshot.getParam<int> (ModifierIDs::RandomVelocityRangeMax, 0);
                            std::uniform_int_distribution<int> velDist (rangeMin, rangeMax);
                            note.velocity = std::clamp (velDist (rng), 0, 127);
                            return note;
                        }),
                    true);
} // namespace
