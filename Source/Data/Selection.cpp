#include "Selection.h"

const std::vector<Position>& Selection::getPositions() const {
    return positions;
}

void Selection::clear() {
    positions.clear();
}

void Selection::toggleLineMode()
{
    if (lineMode == VisualLineMode:: vertical) {
        lineMode = VisualLineMode::horizontal;
    } else if (lineMode == VisualLineMode::horizontal) {
        lineMode = VisualLineMode::vertical;
    }

}

void Selection::addToVisualLineSelection(Position p, Timeline t, Scale s) {
    // If this is the first position, it becomes both cursor and anchor
    if (positions.empty()) {
        anchor = p;
    }

    // Clear existing selection
    positions.clear();

    if (lineMode == VisualLineMode::horizontal) {
        // Get the range of rows to fill (between anchor and cursor)
        double minDegree = std::min(anchor.yDegree.value, p.yDegree.value);
        double maxDegree = std::max(anchor.yDegree.value, p.yDegree.value);

        // For each row in the range...
        for (double d = minDegree; d <= maxDegree; d += s.getSmallestStepSize()) {
            // Fill the entire row
            for (auto i = t.getLowerBound(); i < t.size() - 1; ++i) {
                auto time = i * t.getSmallestStepSize();
                positions.emplace_back(Position{TimePoint{time}, Degree{d}});
            }
        }
    }
    else if (lineMode == VisualLineMode::vertical) {
        // Get the range of columns to fill (between anchor and cursor)
        double minTime = std::min(anchor.xTime.value, p.xTime.value);
        double maxTime = std::max(anchor.xTime.value, p.xTime.value);

        // For each column in the range...
        for (double time = minTime; time <= maxTime; time += t.getSmallestStepSize()) {
            // Fill the entire column
            for (auto i = s.getLowerBound(); i < s.size() - 1; ++i) {
                auto deg = i * s.getSmallestStepSize();
                positions.emplace_back(Position{TimePoint{time}, Degree{deg}});
            }
        }
    }
}

void Selection::addToVisualBlockSelection(Position p) {
    // If this is the first position, it becomes both cursor and anchor
    if (positions.empty()) {
        positions.push_back(p);
        anchor = p;
        return;
    }

    // Clear existing selection and fill in the rectangle between anchor and cursor
    positions.clear();

    // Calculate boundaries between anchor and cursor
    double minTime = std::min(anchor.xTime.value, p.xTime.value);
    double maxTime = std::max(anchor.xTime.value, p.xTime.value);
    double minDegree = std::min(anchor.yDegree.value, p.yDegree.value);
    double maxDegree = std::max(anchor.yDegree.value, p.yDegree.value);

    // Fill in all positions in the rectangle
    for (double t = minTime; t <= maxTime; t += Division::thirtysecond) {
        for (double d = minDegree; d <= maxDegree; d += Division::thirtysecond) {
            Position newPos{TimePoint{t}, Degree{d}};
            positions.push_back(newPos);
        }
    }
}

void Selection::addToSelection(Position p) { positions.emplace_back(p); }

void Selection::removeFromSelection(Position p) {
    // Find the position to remove using std::remove_if with a lambda
    auto newEnd = std::remove_if(positions.begin(), positions.end(),
        [&p](const Position& pos) {
            // Compare both time and degree for exact match
            return Division::isEqual(pos.xTime.value, p.xTime.value) && Division::isEqual(pos.yDegree.value, p.yDegree.value);
        });

    // Erase the removed elements
    positions.erase(newEnd, positions.end());
}

Position Selection::getEarliestPosition() {
    if (positions.empty()) {
        return Position{}; // Return default Position if vector is empty
    }

    return *std::min_element(positions.begin(), positions.end(),
        [](const Position& a, const Position& b) {
            return a.xTime.value < b.xTime.value;
        });
}

Position Selection::getLatestPosition() {
    if (positions.empty()) {
        return Position{};
    }

    return *std::max_element(positions.begin(), positions.end(),
        [](const Position& a, const Position& b) {
            return a.xTime.value < b.xTime.value;
        });
}

Position Selection::getHighestPosition() {
    if (positions.empty()) {
        return Position{};
    }

    return *std::max_element(positions.begin(), positions.end(),
        [](const Position& a, const Position& b) {
            return a.yDegree.value < b.yDegree.value;
        });
}

Position Selection::getLowestPosition() {
    if (positions.empty()) {
        return Position{};
    }

    return *std::min_element(positions.begin(), positions.end(),
        [](const Position& a, const Position& b) {
            return a.yDegree.value < b.yDegree.value;
        });
}

Position Selection::getOppositeCorner(Position p) {
    anchor = p;

    if (positions.empty()) {
        return Position{};
    }

    // First, determine which corner the input position represents
    bool isLatest = Division::isEqual(p.xTime.value, getLatestPosition().xTime.value);
    bool isEarliest = Division::isEqual(p.xTime.value, getEarliestPosition().xTime.value);
    bool isHighest = Division::isEqual(p.yDegree.value, getHighestPosition().yDegree.value);
    bool isLowest = Division::isEqual(p.yDegree.value, getLowestPosition().yDegree.value);

    // Find the opposite corner based on the input position's characteristics
    if (isLatest && isHighest) {
        // If input is top-right, return bottom-left
        return Position{getEarliestPosition().xTime, getLowestPosition().yDegree};
    }
    else if (isLatest && isLowest) {
        // If input is bottom-right, return top-left
        return Position{getEarliestPosition().xTime, getHighestPosition().yDegree};
    }
    else if (isEarliest && isHighest) {
        // If input is top-left, return bottom-right
        return Position{getLatestPosition().xTime, getLowestPosition().yDegree};
    }
    else if (isEarliest && isLowest) {
        // If input is bottom-left, return top-right
        return Position{getLatestPosition().xTime, getHighestPosition().yDegree};
    }

    // If the position isn't at a corner, return default position
    return Position{};
}

void Selection::moveSelection(double step, Direction d)
{
    for (auto& p : positions) {
        switch (d)
        {
            case Direction::left:
                p.xTime.value -= step;
                break;
            case Direction::right:
                p.xTime.value += step;
                break;
            case Direction::up:
                p.yDegree.value += step;
                break;
            case Direction::down:
                p.yDegree.value -= step;
                break;
            default: break;
        }
    }
}
