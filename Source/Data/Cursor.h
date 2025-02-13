/*
  ==============================================================================

    Cursor.h
    Created: 13 Jan 2025 3:09:08pm
    Author:  Danny Keig

  ==============================================================================
*/

# include <JuceHeader.h>
# include <random>
# include "Note.h"
# include "Sequence.h"
# include "Data/Timeline.h"
# include "Data/Scale.h"
# include "Data/Selection.h"
#pragma once

enum class Mode { normal, visualLine, visualBlock, insert, noteEdit };

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

    void removeNote();
    void enableNormalMode();
    void enableVisualLineMode();
    void enableVisualBlockMode();
    void enableInsertMode();
    const Mode getMode() const;
    const juce::String getModeName() const;
    constexpr const char* modeToString(Mode m) const;

    bool isSequenceSelected(const Sequence& seq) const;
    size_t getStepIndex();
    size_t getNoteIndex();

    Sequence& getSequence(size_t index) const;
    Sequence& getSelectedSequence() const;

    void previewNote();
    void previewStep();

    std::vector<MidiNote> extractMidiSequence(size_t seqIndex);

    Timeline timeline{0.0, 4.0};
    Scale scale{"Natural Minor"};

    Position cursorPosition;

    void insertNote();
    void removeNotesAtCursor();

    const juce::String readableCursorPosition() const;

    bool isNormalMode() const;
    bool isVisualLineMode() const;
    bool isVisualBlockMode() const;
    bool isInsertMode() const;


    void jumpForwardBeat();
    void jumpBackBeat();

    void increaseTimelineStepSize();
    void decreaseTimelineStepSize();

    void toggleLineMode();

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
    void selectNote(size_t nIndex);

    Selection visualSelection;

    std::vector<std::reference_wrapper<std::unique_ptr<Note>>> findNotesAtPosition(Position& p, Timeline& t, Scale& s);
};
