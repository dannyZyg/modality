# include <JuceHeader.h>
# include "Data/Timeline.h"
# include "Data/Scale.h"
#pragma once

enum class Direction { up, down, left, right };

struct Position {
    TimePoint xTime = TimePoint{2.0};
    Degree yDegree = Degree{0.0};
};

class Selection {
public:
    Selection() = default;
    ~Selection() = default;

    enum class VisualLineMode { vertical, horizontal };

    void clear();

    void addToVisualLineSelection(Position p, Timeline t, Scale s);
    void addToVisualBlockSelection(Position p);
    void addToSelection(Position p);
    void removeFromSelection(Position p);

    const std::vector<Position>& getPositions() const;
    Position getEarliestPosition();
    Position getLatestPosition();
    Position getHighestPosition();
    Position getLowestPosition();
    Position getOppositeCorner(Position p);

    void moveSelection(double step, Direction d);

    void toggleLineMode();

private:
    std::vector<Position> positions;
    Position anchor;
    VisualLineMode lineMode = VisualLineMode::vertical;
};

