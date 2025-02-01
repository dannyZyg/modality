/*
  ==============================================================================

    Cursor.h
    Created: 13 Jan 2025 3:09:08pm
    Author:  Danny Keig

  ==============================================================================
*/

# include <JuceHeader.h>
# include "Step.h"
# include "Note.h"
# include "Sequence.h"
#pragma once

enum class Mode { normal, visual };

struct SelectionDetails {
    bool isFocused = false;
    bool isVisuallySelected = false;
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
    void toggleStepMute();
    void addNote();
    void removeNote();
    void nextNoteInStep();
    void prevNoteInStep();
    void enableNormalMode();
    void enableVisualMode();
    Mode getMode();
    juce::String getModeName();
    constexpr const char* modeToString(Mode m) throw();

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

private:
    Mode mode = Mode::normal;

    std::vector<std::unique_ptr<Sequence>> sequences;

    size_t selectedSeqIndex = 0;
    size_t selectedStepIndex = 0;
    size_t selectedNoteIndex = 0;
    std::vector<Step*> visualSelection;

    void selectSequence(size_t sIndex);
    void selectStep(size_t sIndex);
    void selectNote(size_t nIndex);
};
