#include "Data/Scale.h"
#include "Data/Timeline.h"
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

    void addToVisualLineSelection (Position p, Timeline t, Scale s);
    void addToVisualBlockSelection (Position p);
    void addToSelection (Position p);
    void removeFromSelection (Position p);

    const std::vector<Position>& getPositions() const;
    Position getEarliestPosition();
    Position getLatestPosition();
    Position getHighestPosition();
    Position getLowestPosition();
    Position getOppositeCorner (Position p);

    void moveSelection (double step, Direction d);

    void toggleLineMode();

private:
    std::vector<Position> positions;
    Position anchor;
    VisualLineMode lineMode = VisualLineMode::vertical;
};
