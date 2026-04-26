/*
  ==============================================================================

    This file contains the basic startup code for a JUCE application.

  ==============================================================================
*/

#include "AppMenuModel.h"
#include "Components/MainComponent.h"
#include <JuceHeader.h>

//==============================================================================
class ModalityApplication : public juce::JUCEApplication
{
public:
    //==============================================================================
    ModalityApplication() {}

    const juce::String getApplicationName() override { return ProjectInfo::projectName; }
    const juce::String getApplicationVersion() override { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override { return true; }

    //==============================================================================
    void initialise ([[maybe_unused]] const juce::String& commandLine) override
    {
        // This method is where you should put your application's initialisation code..

        mainWindow.reset (new MainWindow (getApplicationName()));
    }

    void shutdown() override
    {
        // Add your application's shutdown code here..

        mainWindow = nullptr; // (deletes our window)
    }

    //==============================================================================
    void systemRequestedQuit() override
    {
        if (mainWindow != nullptr)
            mainWindow->tryQuit();
        else
            quit();
    }

    void anotherInstanceStarted ([[maybe_unused]] const juce::String& commandLine) override
    {
        // When another instance of the app is launched while this one is running,
        // this method is invoked, and the commandLine parameter tells you what
        // the other instance's command-line arguments were.
    }

    //==============================================================================
    /*
        This class implements the desktop window that contains an instance of
        our MainComponent class.
    */
    class MainWindow : public juce::DocumentWindow
    {
    public:
        MainWindow (juce::String name)
            : DocumentWindow (name,
                              juce::Desktop::getInstance().getDefaultLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId),
                              DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar (true);
            setContentOwned (new MainComponent(), true);

#if JUCE_IOS || JUCE_ANDROID
            setFullScreen (true);
#else
            setResizable (true, true);
            centreWithSize (getWidth(), getHeight());
#endif

            auto* mc = dynamic_cast<MainComponent*> (getContentComponent());
            appMenuModel = std::make_unique<AppMenuModel> (mc->getComposition(), commandManager);
            commandManager.registerAllCommandsForTarget (appMenuModel.get());
            commandManager.setFirstCommandTarget (appMenuModel.get());
#if JUCE_MAC
            juce::MenuBarModel::setMacMainMenu (appMenuModel.get());
#else
            setMenuBar (appMenuModel.get());
#endif

            setVisible (true);
        }

        ~MainWindow() override
        {
#if JUCE_MAC
            juce::MenuBarModel::setMacMainMenu (nullptr);
#else
            setMenuBar (nullptr);
#endif
        }

        void closeButtonPressed() override
        {
            tryQuit();
        }

        void tryQuit()
        {
            auto* mc = dynamic_cast<MainComponent*> (getContentComponent());
            if (mc->getComposition().isDirty())
            {
                juce::AlertWindow::showYesNoCancelBox (
                    juce::MessageBoxIconType::QuestionIcon,
                    "Unsaved Changes",
                    "You have unsaved changes. Save before closing?",
                    "Save", "Discard", "Cancel",
                    this,
                    juce::ModalCallbackFunction::create ([this] (int result)
                    {
                        if (result == 1) // Save
                        {
                            appMenuModel->setQuitAfterSave (true);
                            commandManager.invokeDirectly (AppMenuModel::FileSave, false);
                        }
                        else if (result == 2) // Discard
                        {
                            JUCEApplication::getInstance()->quit();
                        }
                        // result == 0 (Cancel) → do nothing
                    }));
            }
            else
            {
                JUCEApplication::getInstance()->quit();
            }
        }

        /* Note: Be careful if you override any DocumentWindow methods - the base
           class uses a lot of them, so by overriding you might break its functionality.
           It's best to do all your work in your content component instead, but if
           you really have to override any DocumentWindow methods, make sure your
           subclass also calls the superclass's method.
        */

    private:
        juce::ApplicationCommandManager commandManager;
        std::unique_ptr<AppMenuModel> appMenuModel;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
    };

private:
    std::unique_ptr<MainWindow> mainWindow;
};

//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION (ModalityApplication)
