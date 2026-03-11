#include "Data/Scale.h"
#include "Data/Timeline.h"
#include "juce_core/juce_core.h"
#include <JuceHeader.h>
#pragma once

enum class Direction
{
    up,
    down,
    left,
    right
};

struct Position
{
    TimePoint xTimepoint = TimePoint { 0.0 };
    Degree yDegree = Degree { 0.0 };
};

class Selection
{
public:
    Selection() = default;
    ~Selection() = default;

    enum class VisualLineMode
    {
        vertical,
        horizontal
    };

    enum class MoveMode
    {
        shift,
        extend,
    };

    void clear();

    void addToVisualLineSelection (Position p, Timeline& t, Scale& s);
    void addToVisualBlockSelection (Position p, Timeline& t, Scale& s, bool shouldWrap);
    void addToSelection (Position p);
    void removeFromSelection (Position p);

    const std::vector<Position>& getPositions() const;
    Position getEarliestPosition();
    Position getLatestPosition();
    Position getHighestPosition();
    Position getLowestPosition();
    Position getOppositeCorner (Position p);

    void moveSelection (const Timeline& timeline, const Scale& scale, Direction dir, bool shouldWrap);
    bool canSelectionMove (const Timeline& timeline, const Scale& scale, Direction dir);

    void toggleLineMode();

private:
    std::vector<Position> positions;
    Position anchor;
    VisualLineMode lineMode = VisualLineMode::vertical;
};
