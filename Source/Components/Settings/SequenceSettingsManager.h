#pragma once

#include "Audio/MidiOutputManager.h"
#include "Data/Cursor.h"
#include "Data/MenuNode.h"

class SequenceSettingsManager
{
public:
    SequenceSettingsManager (Cursor& c, MidiOutputManager& m);

    ~SequenceSettingsManager();

    MenuNode* getMenuNodeRoot();

private:
    Cursor& cursor;
    MidiOutputManager& midiOutManager;
    std::unique_ptr<MenuNode> menuRoot;
    MenuNode* midiSettingsNode;

    void buildMidiSettingsMenu();
};
