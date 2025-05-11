#include "Scale.h"

Degree::Degree (double init)
{
    value = init;
}

Scale::Scale (const std::string& name)
{
    // Find the matching scale definition
    auto it = std::find_if (scaleDefinitions.begin(), scaleDefinitions.end(), [&name] (const auto& def)
                            { return def.first == name; });

    if (it != scaleDefinitions.end())
    {
        degrees = it->second;
    }
}

Degree Scale::getHigher (const Degree& d)
{
    auto descendingDegrees = getDescendingDegrees();

    if (d.value >= 0.0)
    {
        // For zero or positive values, find in ascending sequence
        auto it = std::lower_bound (degrees.begin(), degrees.end(), d.value);
        if (it != degrees.end() && std::next (it) != degrees.end())
        {
            return Degree (*std::next (it));
        }
        return Degree (d.value); // At highest value, stay there
    }
    else
    {
        // For negative values, find this value in descending sequence
        auto it = std::find_if (descendingDegrees.begin(), descendingDegrees.end(), [d] (double val)
                                { return std::abs (val - d.value) < 0.0001; });

        if (it != descendingDegrees.end())
        {
            if (it == descendingDegrees.begin())
            {
                return Degree (0.0); // Return to zero
            }
            return Degree (*std::prev (it)); // Move up in descending sequence
        }
        return Degree (d.value); // Not found, stay at current value
    }
}

Degree Scale::getLower (const Degree& d)
{
    // Get the descending scale degrees
    auto descendingDegrees = getDescendingDegrees();

    if (d.value > 0.0)
    {
        // For positive values, first try to find a lower positive degree
        auto it = std::lower_bound (degrees.begin(), degrees.end(), d.value);
        if (it != degrees.begin())
        {
            return Degree (*(--it));
        }
        // If we can't go lower in positive degrees, return first negative
        return Degree (descendingDegrees[1]); // [0] is 0.0, [1] is first negative
    }
    else if (d.value == 0.0)
    {
        // Special case for 0.0 - return first negative degree
        return Degree (descendingDegrees[1]);
    }
    else
    {
        // For negative values, find this value in descending sequence
        auto it = std::find_if (descendingDegrees.begin(), descendingDegrees.end(), [d] (double val)
                                { return std::abs (val - d.value) < 0.0001; });

        if (it != descendingDegrees.end() && std::next (it) != descendingDegrees.end())
        {
            return Degree (*std::next (it));
        }
        return Degree (d.value); // At lowest value, stay there
    }
}

double Scale::getUpperBound() const
{
    if (! degrees.empty())
    {
        return degrees.back();
    }
    return 0.0;
}

double Scale::getLowerBound() const
{
    auto descendingDegrees = getDescendingDegrees();
    if (! descendingDegrees.empty())
    {
        return descendingDegrees.back();
    }
    return 0.0;
}

double Scale::getSmallestStepSize() const
{
    double smallest = std::numeric_limits<double>::max();
    for (size_t i = 1; i < degrees.size(); ++i)
    {
        double step = degrees[i] - degrees[i - 1];
        smallest = std::min (smallest, step);
    }
    return smallest;
}

double Scale::getStepSize() const
{
    return getSmallestStepSize();
}

double Scale::size() const
{
    if (! degrees.empty())
    {
        return degrees.back() - (-degrees.back()) + 1;
    }
    return 0.0;
}

std::vector<double> Scale::getDescendingDegrees() const
{
    std::vector<double> descending;
    if (degrees.empty())
        return descending;

    // Start at 0
    descending.push_back (0.0);

    // Get the intervals by looking at gaps between scale degrees at the top octave
    std::vector<double> intervals;
    for (size_t i = degrees.size() - 1; i > 0; --i)
    {
        intervals.push_back (degrees[i] - degrees[i - 1]);
    }

    // Apply these intervals in reverse to get negative degrees
    double current = 0.0;
    for (double interval : intervals)
    {
        current -= interval;
        descending.push_back (current);
    }

    return descending;
}
