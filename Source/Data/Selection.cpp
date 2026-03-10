#include "Selection.h"
#include "Data/Scale.h"

const std::vector<Position>& Selection::getPositions() const
{
    return positions;
}

void Selection::clear()
{
    positions.clear();
}

void Selection::toggleLineMode()
{
    if (lineMode == VisualLineMode::vertical)
    {
        lineMode = VisualLineMode::horizontal;
    }
    else if (lineMode == VisualLineMode::horizontal)
    {
        lineMode = VisualLineMode::vertical;
    }
}

void Selection::addToVisualLineSelection (Position pos, Timeline& timeline, Scale& scale)
{
    // If this is the first position, it becomes both cursor and anchor
    if (positions.empty())
    {
        anchor = pos;
    }

    // Clear existing selection
    positions.clear();

    if (lineMode == VisualLineMode::horizontal)
    {
        // Get the range of rows to fill (between anchor and cursor)
        double minDegree = std::min (anchor.yDegree.value, pos.yDegree.value);
        double maxDegree = std::max (anchor.yDegree.value, pos.yDegree.value);

        // For each row in the range...
        for (double d = minDegree; d <= maxDegree; d += scale.getSmallestStepSize())
        {
            // Fill the entire row
            for (auto i = timeline.getLowerBound(); i < timeline.size() - 1; ++i)
            {
                auto time = i * timeline.getSmallestStepSize();
                positions.emplace_back (Position { TimePoint { time }, Degree { d } });
            }
        }
    }
    else if (lineMode == VisualLineMode::vertical)
    {
        // Get the range of columns to fill (between anchor and cursor)
        double minTime = std::min (anchor.xTimepoint.value, pos.xTimepoint.value);
        double maxTime = std::max (anchor.xTimepoint.value, pos.xTimepoint.value);

        // For each column in the range...
        for (double time = minTime; time <= maxTime; time += timeline.getSmallestStepSize())
        {
            // Fill the entire column
            for (auto i = scale.getLowerBound(); i < scale.size() - 1; ++i)
            {
                auto deg = i * scale.getSmallestStepSize();
                positions.emplace_back (Position { TimePoint { time }, Degree { deg } });
            }
        }
    }
}

void Selection::addToVisualBlockSelection (Position pos, Timeline& timeline, Scale& scale)
{
    // If this is the first position, it becomes both cursor and anchor
    if (positions.empty())
    {
        positions.push_back (pos);
        anchor = pos;
        return;
    }

    // Clear existing selection and fill in the rectangle between anchor and cursor
    positions.clear();

    // Calculate boundaries between anchor and cursor
    double minTime = std::min (anchor.xTimepoint.value, pos.xTimepoint.value);
    double maxTime = std::max (anchor.xTimepoint.value, pos.xTimepoint.value);
    double minDegree = std::min (anchor.yDegree.value, pos.yDegree.value);
    double maxDegree = std::max (anchor.yDegree.value, pos.yDegree.value);

    // Fill in all positions in the rectangle
    for (double t = minTime; t <= maxTime; t += timeline.getStepSize())
    {
        Degree currentDegree (minDegree);

        while (currentDegree.value <= maxDegree)
        {
            positions.push_back (Position { TimePoint { t }, Degree { currentDegree } });
            Degree nextDegree = scale.getHigher (currentDegree);
            if (juce::approximatelyEqual (nextDegree.value, currentDegree.value))
                break;
            currentDegree = nextDegree;
        }
    }
}

void Selection::addToSelection (Position p) { positions.emplace_back (p); }

void Selection::removeFromSelection (Position p)
{
    // Find the position to remove using std::remove_if with a lambda
    auto newEnd = std::remove_if (positions.begin(), positions.end(), [&p] (const Position& pos)
                                  {
            // Compare both time and degree for exact match
            return Division::isEqual(pos.xTimepoint.value, p.xTimepoint.value) && Division::isEqual(pos.yDegree.value, p.yDegree.value); });

    // Erase the removed elements
    positions.erase (newEnd, positions.end());
}

Position Selection::getEarliestPosition()
{
    if (positions.empty())
    {
        return Position {}; // Return default Position if vector is empty
    }

    return *std::min_element (positions.begin(), positions.end(), [] (const Position& a, const Position& b)
                              { return a.xTimepoint.value < b.xTimepoint.value; });
}

Position Selection::getLatestPosition()
{
    if (positions.empty())
    {
        return Position {};
    }

    return *std::max_element (positions.begin(), positions.end(), [] (const Position& a, const Position& b)
                              { return a.xTimepoint.value < b.xTimepoint.value; });
}

Position Selection::getHighestPosition()
{
    if (positions.empty())
    {
        return Position {};
    }

    return *std::max_element (positions.begin(), positions.end(), [] (const Position& a, const Position& b)
                              { return a.yDegree.value < b.yDegree.value; });
}

Position Selection::getLowestPosition()
{
    if (positions.empty())
    {
        return Position {};
    }

    return *std::min_element (positions.begin(), positions.end(), [] (const Position& a, const Position& b)
                              { return a.yDegree.value < b.yDegree.value; });
}

Position Selection::getOppositeCorner (Position p)
{
    anchor = p;

    if (positions.empty())
    {
        return Position {};
    }

    // First, determine which corner the input position represents
    bool isLatest = Division::isEqual (p.xTimepoint.value, getLatestPosition().xTimepoint.value);
    bool isEarliest = Division::isEqual (p.xTimepoint.value, getEarliestPosition().xTimepoint.value);
    bool isHighest = Division::isEqual (p.yDegree.value, getHighestPosition().yDegree.value);
    bool isLowest = Division::isEqual (p.yDegree.value, getLowestPosition().yDegree.value);

    // Find the opposite corner based on the input position's characteristics
    if (isLatest && isHighest)
    {
        // If input is top-right, return bottom-left
        return Position { getEarliestPosition().xTimepoint, getLowestPosition().yDegree };
    }
    else if (isLatest && isLowest)
    {
        // If input is bottom-right, return top-left
        return Position { getEarliestPosition().xTimepoint, getHighestPosition().yDegree };
    }
    else if (isEarliest && isHighest)
    {
        // If input is top-left, return bottom-right
        return Position { getLatestPosition().xTimepoint, getLowestPosition().yDegree };
    }
    else if (isEarliest && isLowest)
    {
        // If input is bottom-left, return top-right
        return Position { getLatestPosition().xTimepoint, getHighestPosition().yDegree };
    }

    // If the position isn't at a corner, return default position
    return Position {};
}

void Selection::moveSelection (const Timeline& timeline, const Scale& scale, Direction d)
{
    for (auto& p : positions)
    {
        switch (d)
        {
            case Direction::left:
                p.xTimepoint.value -= timeline.getStepSize();
                break;
            case Direction::right:
                p.xTimepoint.value += timeline.getStepSize();
                break;
            case Direction::up:
                p.yDegree = scale.getHigher (p.yDegree.value);
                break;
            case Direction::down:
                p.yDegree = scale.getLower (p.yDegree.value);
                break;
            default:
                break;
        }
    }
}
