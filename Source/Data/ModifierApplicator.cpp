#include "ModifierApplicator.h"
#include "Data/Modifier.h"
#include "Data/Scale.h"
#include <algorithm>
#include <random>

namespace
{
static std::mt19937 rng { std::random_device {}() };

} // namespace
