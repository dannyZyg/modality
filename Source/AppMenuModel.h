#pragma once

#include "Data/Composition.h"
#include <JuceHeader.h>

class AppMenuModel : public juce::MenuBarModel,
                     public juce::ApplicationCommandTarget
{
public:
    enum CommandIDs
    {
        FileNew = 1,
        FileOpen = 2,
        FileSave = 3,
        FileSaveAs = 4
    };

    AppMenuModel (Composition& composition, juce::ApplicationCommandManager& commandManager);
    ~AppMenuModel() override = default;

    //==============================================================================
    // MenuBarModel
    juce::StringArray getMenuBarNames() override;
    juce::PopupMenu getMenuForIndex (int menuIndex, const juce::String& menuName) override;
    void menuItemSelected (int menuItemID, int topLevelMenuIndex) override;

    //==============================================================================
    // ApplicationCommandTarget
    juce::ApplicationCommandTarget* getNextCommandTarget() override;
    void getAllCommands (juce::Array<juce::CommandID>& commands) override;
    void getCommandInfo (juce::CommandID commandID, juce::ApplicationCommandInfo& result) override;
    bool perform (const juce::ApplicationCommandTarget::InvocationInfo& info) override;

    void setQuitAfterSave (bool v) { quitAfterSave = v; }

private:
    Composition& composition;
    juce::ApplicationCommandManager& commandManager;
    std::unique_ptr<juce::FileChooser> fileChooser;
    bool quitAfterSave { false };

    void doNew();
    void doOpen();
    void doSave();
    void doSaveAs();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AppMenuModel)
};
