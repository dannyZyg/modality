/*
  ==============================================================================

    Cursor.h
    Created: 13 Jan 2025 3:09:08pm
    Author:  Danny Keig

  ==============================================================================
*/

# include <JuceHeader.h>
# include <random>
# include "Step.h"
# include "Note.h"
# include "Sequence.h"
# include "Data/Timeline.h"
# include "Data/Scale.h"
#pragma once

enum class Mode { normal, visualLine, visualBlock, insert };

enum class Direction { up, down, left, right };

struct SelectionDetails {
    bool isFocused = false;
    bool isVisuallySelected = false;
};

struct Position {
    TimePoint xTime = TimePoint{2.0};
    Degree yDegree = Degree{0.0};
};

class Selection {
public:
    Selection() = default;
    ~Selection() = default;

    enum class VisualLineMode { vertical, horizontal };

    const std::vector<Position>& getPositions() const {
        return positions;
    }

    void clear() {
        positions.clear();
    }

    void addToVisualLineSelection(Position p, VisualLineMode m, Timeline t, Scale s)
    {
        if (m == VisualLineMode::horizontal) {
            // Add all time positions from 0 to the end, at the smallest interval (a whole row)
            for (auto i = t.getLowerBound(); i < t.size() - 1; ++i) {
                auto time = i * t.getSmallestStepSize();
                positions.emplace_back(Position{TimePoint{time}, Degree{p.yDegree.value}});
            }
        }

        if (m == VisualLineMode::vertical) {
            // Add all degree positions from the lowest to the highest, at the smallest interval (a whole column)
            for (auto i = s.getLowerBound(); i < s.size() - 1; ++i) {
                auto deg = i * s.getSmallestStepSize();
                positions.emplace_back(Position{TimePoint{p.xTime.value}, Degree{deg}});
            }
        }
    }

    void addToBlockSelection(Position p) {
        // Get the opposite corner to form the block
        Position cornerA = p;
        Position cornerB;

        // If this is the first position, just add it
        if (positions.empty()) {
            positions.emplace_back(p);
            return;
        }

        // Use the earliest added position as the other corner
        cornerB = positions.front();

        // Calculate the block boundaries
        double startTime = std::min(cornerA.xTime.value, cornerB.xTime.value);
        double endTime = std::max(cornerA.xTime.value, cornerB.xTime.value);
        double lowDegree = std::min(cornerA.yDegree.value, cornerB.yDegree.value);
        double highDegree = std::max(cornerA.yDegree.value, cornerB.yDegree.value);

        // Clear existing selection
        positions.clear();

        // Add all positions in the block
        for (double t = startTime; Division::isLessOrEqual(t, endTime); t += 1.0) {
            for (double d = lowDegree; Division::isLessOrEqual(d, highDegree); d += 1.0) {
                positions.emplace_back(Position{TimePoint{t}, Degree{d}});
            }
        }
    }

    void removeFromBlockSelection(Position p) {
        // For block mode, removing a position effectively means
        // creating a new block selection that excludes that point
        Position cornerA = p;

        // If this is the last position or no positions, just clear
        if (positions.size() <= 1) {
            positions.clear();
            return;
        }

        // Use the earliest added position as the other corner
        Position cornerB = positions.front();

        // Calculate the new block boundaries
        double startTime = std::min(cornerA.xTime.value, cornerB.xTime.value);
        double endTime = std::max(cornerA.xTime.value, cornerB.xTime.value);
        double lowDegree = std::min(cornerA.yDegree.value, cornerB.yDegree.value);
        double highDegree = std::max(cornerA.yDegree.value, cornerB.yDegree.value);

        // Clear existing selection
        positions.clear();

        // Add all positions in the block except the removed position
        for (double t = startTime; Division::isLessOrEqual(t, endTime); t += 1.0) {
            for (double d = lowDegree; Division::isLessOrEqual(d, highDegree); d += 1.0) {
                Position newPos{TimePoint{t}, Degree{d}};
                if (!Division::isEqual(newPos.xTime.value, p.xTime.value) ||
                    !Division::isEqual(newPos.yDegree.value, p.yDegree.value)) {
                    positions.emplace_back(newPos);
                }
            }
        }
    }

    void addToSelection(Position p) { positions.emplace_back(p); }

    void removeFromSelection(Position p) {
        // Find the position to remove using std::remove_if with a lambda
        auto newEnd = std::remove_if(positions.begin(), positions.end(),
            [&p](const Position& pos) {
                // Compare both time and degree for exact match
                return Division::isEqual(pos.xTime.value, p.xTime.value) && Division::isEqual(pos.yDegree.value, p.yDegree.value);
            });

        // Erase the removed elements
        positions.erase(newEnd, positions.end());
    }

    Position getEarliestPosition() {
        if (positions.empty()) {
            return Position{}; // Return default Position if vector is empty
        }

        return *std::min_element(positions.begin(), positions.end(),
            [](const Position& a, const Position& b) {
                return a.xTime.value < b.xTime.value;
            });
    }

    Position getLatestPosition() {
        if (positions.empty()) {
            return Position{};
        }

        return *std::max_element(positions.begin(), positions.end(),
            [](const Position& a, const Position& b) {
                return a.xTime.value < b.xTime.value;
            });
    }

    Position getHighestPosition() {
        if (positions.empty()) {
            return Position{};
        }

        return *std::max_element(positions.begin(), positions.end(),
            [](const Position& a, const Position& b) {
                return a.yDegree.value < b.yDegree.value;
            });
    }

    Position getLowestPosition() {
        if (positions.empty()) {
            return Position{};
        }

        return *std::min_element(positions.begin(), positions.end(),
            [](const Position& a, const Position& b) {
                return a.yDegree.value < b.yDegree.value;
            });
    }

    Position getOppositeCorner(Position p) {
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

    void moveSelection(double step, Direction d)
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

private:
    std::vector<Position> positions;
};

class Cursor
{

public:
    Cursor();
    ~Cursor();

    void createSequence();

    void setVisualSelection();
    void jumpToStart();
    void jumpToEnd();

    void moveRight();
    void moveLeft();
    void moveDown();
    void moveUp();

    void moveCursorSelection(Direction d);
    void moveNotesInSelection(Direction d);

    void toggleStepMute();
    void removeNote();
    void nextNoteInStep();
    void prevNoteInStep();
    void enableNormalMode();
    void enableVisualLineMode();
    void enableVisualBlockMode();
    void enableInsertMode();
    Mode getMode();
    const juce::String getModeName() const;
    constexpr const char* modeToString(Mode m) const;

    bool isSequenceSelected(const Sequence& seq) const;
    bool isStepSelected(const Step& step) const;
    bool isNoteSelected(const Note& note) const;
    bool isStepVisuallySelected(const Step& step) const;

    SelectionDetails getSelectionDetails(const Step& step) const;
    size_t getStepIndex();
    size_t getNoteIndex();
    Step& getCurrentStep();

    Sequence& getSequence(size_t index) const;
    Sequence& getSelectedSequence() const;

    void previewNote();
    void previewStep();

    std::vector<Sequence::MidiNote> extractMidiSequence(size_t seqIndex);

    Timeline timeline{0.0, 4.0};
    Scale scale{-12.0, 12.0};

    Position cursorPosition;

    void insertNote();
    void removeNotesAtCursor();

    juce::String readableCursorPosition();

    bool isNormalMode() const;
    bool isVisualLineMode() const;
    bool isVisualBlockMode() const;
    bool isInsertMode() const;


    void jumpForwardBeat();
    void jumpBackBeat();

    void increaseTimelineStepSize();
    void decreaseTimelineStepSize();


    const std::vector<Position>& getVisualSelectionPositions() const;
    Position getVisualSelectionOpposite();

    void addModifier();




private:
    std::mt19937 randomGenerator;

    Mode mode = Mode::normal;

    std::vector<std::unique_ptr<Sequence>> sequences;

    size_t selectedSeqIndex = 0;
    size_t selectedStepIndex = 0;
    size_t selectedNoteIndex = 0;

    void selectSequence(size_t sIndex);
    void selectStep(size_t sIndex);
    void selectNote(size_t nIndex);

    Selection visualSelection;

    std::vector<std::reference_wrapper<std::unique_ptr<Note>>> findNotesAtPosition(Position& p, Timeline& t, Scale& s);
};
