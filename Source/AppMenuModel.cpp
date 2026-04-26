#include "AppMenuModel.h"

AppMenuModel::AppMenuModel (Composition& c, juce::ApplicationCommandManager& cm)
    : composition (c), commandManager (cm)
{
    setApplicationCommandManagerToWatch (&commandManager);
}

juce::StringArray AppMenuModel::getMenuBarNames()
{
    return { "File" };
}

juce::PopupMenu AppMenuModel::getMenuForIndex ([[maybe_unused]] int menuIndex,
                                               [[maybe_unused]] const juce::String& menuName)
{
    juce::PopupMenu menu;
    menu.addCommandItem (&commandManager, FileNew);
    menu.addSeparator();
    menu.addCommandItem (&commandManager, FileOpen);
    menu.addSeparator();
    menu.addCommandItem (&commandManager, FileSave);
    menu.addCommandItem (&commandManager, FileSaveAs);
    return menu;
}

void AppMenuModel::menuItemSelected ([[maybe_unused]] int menuItemID,
                                     [[maybe_unused]] int topLevelMenuIndex)
{
    // Not needed — ApplicationCommandManager handles dispatch.
}

juce::ApplicationCommandTarget* AppMenuModel::getNextCommandTarget()
{
    return nullptr;
}

void AppMenuModel::getAllCommands (juce::Array<juce::CommandID>& commands)
{
    commands.addArray ({ FileNew, FileOpen, FileSave, FileSaveAs });
}

void AppMenuModel::getCommandInfo (juce::CommandID commandID, juce::ApplicationCommandInfo& result)
{
    switch (commandID)
    {
        case FileNew:
            result.setInfo ("New", "Create a new composition", "File", 0);
            result.addDefaultKeypress ('n', juce::ModifierKeys::commandModifier);
            break;

        case FileOpen:
            result.setInfo ("Open...", "Open a composition from disk", "File", 0);
            result.addDefaultKeypress ('o', juce::ModifierKeys::commandModifier);
            break;

        case FileSave:
            result.setInfo ("Save", "Save the current composition", "File", 0);
            result.addDefaultKeypress ('s', juce::ModifierKeys::commandModifier);
            result.setActive (composition.isDirty() || composition.hasFile());
            break;

        case FileSaveAs:
            result.setInfo ("Save As...", "Save the composition to a new file", "File", 0);
            result.addDefaultKeypress ('s', juce::ModifierKeys::commandModifier | juce::ModifierKeys::shiftModifier);
            break;

        default:
            break;
    }
}

bool AppMenuModel::perform (const juce::ApplicationCommandTarget::InvocationInfo& info)
{
    switch (info.commandID)
    {
        case FileNew:
            doNew();
            return true;
        case FileOpen:
            doOpen();
            return true;
        case FileSave:
            doSave();
            return true;
        case FileSaveAs:
            doSaveAs();
            return true;
        default:
            return false;
    }
}

void AppMenuModel::doNew()
{
    if (composition.isDirty())
    {
        juce::AlertWindow::showYesNoCancelBox (
            juce::MessageBoxIconType::QuestionIcon,
            "Unsaved Changes",
            "You have unsaved changes. Save before creating a new composition?",
            "Save",
            "Discard",
            "Cancel",
            nullptr,
            juce::ModalCallbackFunction::create ([this] (int result)
                                                 {
                if (result == 1)
                    doSave();

                if (result == 1 || result == 2)
                    composition.reset(); }));
        return;
    }

    composition.reset();
}

void AppMenuModel::doOpen()
{
    fileChooser = std::make_unique<juce::FileChooser> (
        "Open Composition",
        juce::File::getSpecialLocation (juce::File::userDocumentsDirectory),
        "*.modality");

    fileChooser->launchAsync (
        juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this] (const juce::FileChooser& fc)
        {
            auto file = fc.getResult();
            if (file != juce::File {})
                composition.loadFromFile (file);
        });
}

void AppMenuModel::doSave()
{
    if (composition.hasFile())
    {
        composition.save();
        if (quitAfterSave)
        {
            quitAfterSave = false;
            JUCEApplication::getInstance()->quit();
        }
    }
    else
    {
        doSaveAs();
    }
}

void AppMenuModel::doSaveAs()
{
    fileChooser = std::make_unique<juce::FileChooser> (
        "Save Composition",
        juce::File::getSpecialLocation (juce::File::userDocumentsDirectory),
        "*.modality");

    fileChooser->launchAsync (
        juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles,
        [this] (const juce::FileChooser& fc)
        {
            auto file = fc.getResult();
            if (file != juce::File {})
            {
                composition.saveToFile (file);
                if (quitAfterSave)
                {
                    quitAfterSave = false;
                    JUCEApplication::getInstance()->quit();
                }
            }
            else
            {
                // User cancelled the file chooser — don't quit
                quitAfterSave = false;
            }
        });
}
