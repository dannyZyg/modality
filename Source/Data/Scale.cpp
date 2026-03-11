#include "Scale.h"
#include "juce_data_structures/juce_data_structures.h"
#include <algorithm>
#include <utility>

Degree::Degree (double init)
{
    value = init;
}

Scale::Scale (const juce::String& name) : state (ScaleIDs::Scale)
{
    setScale (name);
}

Scale::Scale (juce::ValueTree existingState) : state (existingState.isValid() ? std::move (existingState) : juce::ValueTree (ScaleIDs::Scale))
{
    if (! state.hasProperty (ScaleIDs::Name))
        setScale ("Major");
}

juce::ValueTree& Scale::getState() { return state; }

void Scale::setScale (juce::String scaleName, juce::UndoManager* undoManager)
{
    // Find the matching scale definition
    auto scaleDef = std::find_if (scaleDefinitions.begin(), scaleDefinitions.end(), [&scaleName] (const auto& def)
                                  { return def.first == scaleName; });

    if (scaleDef != scaleDefinitions.end())
    {
        setName (scaleDef->first, undoManager);
        setDegrees (scaleDef->second, undoManager);
    }
}

juce::String Scale::getName() const
{
    return state.getProperty (ScaleIDs::Name);
}

void Scale::setName (juce::String name, juce::UndoManager* undoManager)
{
    state.setProperty (ScaleIDs::Name, name, undoManager);
}

void Scale::setDegrees (std::vector<double> newDegrees, juce::UndoManager* undoManager)
{
    juce::Array<var> degreesAsVar;

    for (const auto& item : newDegrees)
    {
        degreesAsVar.add (item);
    }
    state.setProperty (ScaleIDs::Degrees, degreesAsVar, undoManager);
}

std::vector<double> Scale::getDegrees() const
{
    std::vector<double> result;

    if (auto* arr = state.getProperty (ScaleIDs::Degrees).getArray())
    {
        for (const auto& item : *arr)
        {
            result.push_back (static_cast<double> (item));
        }
    }
    return result;
}

const Degree Scale::getHigher (const Degree& d, bool shouldWrap) const
{
    auto descendingDegrees = getDescendingDegrees();
    auto degrees = getDegrees();

    if (d.value >= 0.0)
    {
        // For zero or positive values, find in ascending sequence
        auto it = std::upper_bound (degrees.begin(), degrees.end(), d.value);

        if (it != degrees.end())
        {
            return Degree (*it);
        }

        if (shouldWrap)
            return Degree (getLowerBound());

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

const Degree Scale::getLower (const Degree& d, bool shouldWrap) const
{
    // Get the descending scale degrees
    auto descendingDegrees = getDescendingDegrees();
    auto degrees = getDegrees();

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

        if (shouldWrap)
            return Degree (getUpperBound());

        return Degree (d.value); // At lowest value, stay there
    }
}

double Scale::getUpperBound() const
{
    auto degrees = getDegrees();
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
    auto degrees = getDegrees();
    double smallest = std::numeric_limits<double>::max();
    for (size_t i = 1; i < degrees.size(); ++i)
    {
        double step = degrees[i] - degrees[i - 1];
        smallest = std::min (smallest, step);
    }
    return smallest;
}

double Scale::size() const
{
    auto degrees = getDegrees();
    if (! degrees.empty())
    {
        return degrees.back() - (-degrees.back()) + 1;
    }
    return 0.0;
}

std::vector<double> Scale::getDescendingDegrees() const
{
    std::vector<double> descending;
    auto degrees = getDegrees();
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
