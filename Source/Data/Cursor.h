/*
  ==============================================================================

    Cursor.h
    Created: 13 Jan 2025 3:09:08pm
    Author:  Danny Keig

  ==============================================================================
*/

#include "Data/Scale.h"
#include "Data/Selection.h"
#include "Data/Timeline.h"
#include "Note.h"
#include "Sequence.h"
#include <JuceHeader.h>
#include <random>
#pragma once

enum class Mode
{
    normal,
    visualLine,
    visualBlock,
    insert,
    noteEdit
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

    void move (Direction d, Selection::MoveMode moveMode = Selection::MoveMode::extend);

    void moveCursorSelection (Direction d);
    void moveNotesInSelection (Direction d);

    void removeNote();
    void enableNormalMode();
    void enableVisualLineMode();
    void enableVisualBlockMode();
    void enableInsertMode();
    const Mode getMode() const;
    const juce::String getModeName() const;
    constexpr const char* modeToString (Mode m) const;

    bool isSequenceSelected (const Sequence& seq) const;
    size_t getStepIndex();
    size_t getNoteIndex();

    void selectSequence (size_t sIndex);
    Sequence& getSequence (size_t index) const;
    Sequence& getSelectedSequence() const;

    const std::vector<std::unique_ptr<Sequence>>& getSequences() const;

    void previewNote();
    void previewStep();

    std::vector<MidiNote> extractMidiSequence (size_t seqIndex);

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

    int addModifier (ModifierType t);
    int removeModifier (ModifierType t);

    std::vector<std::reference_wrapper<std::unique_ptr<Note>>> findNotesAtCursor();
    std::vector<std::reference_wrapper<std::unique_ptr<Note>>> findNotesInCursorSelection();
    std::vector<std::reference_wrapper<std::unique_ptr<Note>>> findNotesForCursorMode();

    const Timeline& getCurrentTimeline() const;
    const Scale& getCurrentScale() const;
    Timeline& getCurrentTimeline();
    Scale& getCurrentScale();

private:
    std::mt19937 randomGenerator;

    Mode mode = Mode::normal;

    int numInitialSequences = 4;
    std::vector<std::unique_ptr<Sequence>> sequences;

    size_t selectedSeqIndex = 0;
    size_t selectedStepIndex = 0;
    size_t selectedNoteIndex = 0;

    void selectNote (size_t nIndex);

    Selection visualSelection;

    std::vector<std::reference_wrapper<std::unique_ptr<Note>>> findNotesAtPosition (Position& p, Timeline& t, Scale& s);
};
