#include "Components/ModifierMenuComponent.h"
#include "Data/Modifier.h"
#include "Data/ModifierRegistry.h"
#include <JuceHeader.h>

//==============================================================================
ModifierMenuComponent::ModifierMenuComponent (Cursor& c) : ContextualMenuComponent(), cursor (c)
{
    setVisible (false);
    //setWantsKeyboardFocus(true);
}

ModifierMenuComponent::~ModifierMenuComponent()
{
}

void ModifierMenuComponent::drawContextualContent (juce::Graphics& g)
{
    // Draw based on the current mode
    switch (mode)
    {
        case ModMenuMode::top:
            drawTopMode (g);
            break;
        case ModMenuMode::edit:
            drawEditMode (g);
            break;
        case ModMenuMode::add:
            drawModifierOptions (g);
            break;
        case ModMenuMode::remove:
            drawRemoveMode (g);
            break;
        case ModMenuMode::complete:
            // Draw complete message
            g.setColour (juce::Colours::white);
            g.setFont (20.0f);
            g.drawText ("Operation Complete", getLocalBounds(), juce::Justification::centred);
            break;
    }
}

void ModifierMenuComponent::drawTopMode (juce::Graphics& g)
{
    // Your existing implementation
    g.setColour (juce::Colours::white);
    g.setFont (16.0f);
    g.drawText ("Modifier Menu", getLocalBounds().reduced (10), juce::Justification::centredTop);

    // Additional drawing code...
}

void ModifierMenuComponent::drawModifierOptions (juce::Graphics& g)
{
    // Your existing implementation
    // Draw available modifier options
}

void ModifierMenuComponent::drawEditMode (juce::Graphics& g)
{
    // Your existing implementation
    // Draw edit mode UI
}

void ModifierMenuComponent::drawRemoveMode (juce::Graphics& g)
{
    // Your existing implementation
    // Draw remove mode UI
}

/* void ModifierMenuComponent::paint (juce::Graphics& g) */
/* { */
/*     g.fillAll(juce::Colours::lightslategrey.withLightness(0.7f).withAlpha(0.9f)); */
/*     g.setColour(juce::Colours::white); */
/**/
/*     g.setColour(juce::Colours::black); */
/*     g.drawRect (getLocalBounds(), 1);   // draw an outline around the component */
/**/
/*     g.setFont(juce::Font(juce::Font::getDefaultMonospacedFontName(), 20.0f, juce::Font::plain)); */
/**/
/*     // Set text color */
/*     g.setColour(juce::Colours::black);  // Or any color you prefer */
/**/
/*     // Draw the text centered at the top */
/*     g.drawText("Modifier Menu", */
/*                getLocalBounds().removeFromTop(30),  // Take top 30 pixels of component */
/*                juce::Justification::centred,        // Center horizontally and vertically */
/*                false); */
/**/
/**/
/*     if (mode == ModMenuMode::top) { */
/*         drawTopMode(g); */
/*     } */
/*     if (mode == ModMenuMode::add || mode == ModMenuMode::edit || mode == ModMenuMode::remove) { */
/*         drawModifierOptions(g); */
/*     } */
/**/
/*     if (mode == ModMenuMode::complete) { */
/*         g.setFont(juce::Font(juce::Font::getDefaultMonospacedFontName(), 16.0f, juce::Font::plain)); */
/*         g.drawText(completeMessage, */
/*                    getLocalBounds().removeFromTop(200), */
/*                    juce::Justification::centred,        // Center horizontally and vertically */
/*                    false); */
/*     } */
/* } */

/* void ModifierMenuComponent::drawTopMode(juce::Graphics& g) */
/* { */
/*     g.setFont(juce::Font(juce::Font::getDefaultMonospacedFontName(), 16.0f, juce::Font::plain)); */
/*     int startY = 50; */
/*     int itemHeight = 25; */
/*     int labelWidth = 30;      // Width for the single letter */
/*     int descriptionX = 100;   // Fixed position where descriptions start */
/**/
/*     // Pairs of label and description */
/*     struct MenuItem { */
/*         juce::String label; */
/*         juce::String description; */
/*     }; */
/**/
/*     std::vector<MenuItem> items = { */
/*         {"a", "add modifier"}, */
/*         {"e", "edit modifier"}, */
/*         {"r", "remove modifier"} */
/*     }; */
/**/
/*     for (int i = 0; i < items.size(); ++i) */
/*     { */
/*         // Draw the label (single letter) */
/*         juce::Rectangle<int> labelBounds( */
/*             20,                         // Left margin */
/*             startY + (i * itemHeight), */
/*             labelWidth, */
/*             itemHeight */
/*         ); */
/**/
/*         // Draw the description (aligned at fixed position) */
/*         juce::Rectangle<int> descBounds( */
/*             descriptionX, */
/*             startY + (i * itemHeight), */
/*             getWidth() - descriptionX - 20,  // Right margin of 20 */
/*             itemHeight */
/*         ); */
/**/
/*         g.drawText(items[i].label, */
/*                   labelBounds, */
/*                   juce::Justification::left, */
/*                   false); */
/**/
/*         g.drawText(items[i].description, */
/*                   descBounds, */
/*                   juce::Justification::left, */
/*                   false); */
/*     } */
/**/
/* } */

/* void ModifierMenuComponent::drawModifierOptions(juce::Graphics& g) */
/* { */
/*     g.setFont(juce::Font(juce::Font::getDefaultMonospacedFontName(), 16.0f, juce::Font::plain)); */
/*     int startY = 50; */
/*     int itemHeight = 25; */
/*     int labelWidth = 30;      // Width for the single letter */
/*     int descriptionX = 100;   // Fixed position where descriptions start */
/**/
/*     // Pairs of label and description */
/*     struct MenuItem { */
/*         juce::String label; */
/*         juce::String description; */
/*     }; */
/**/
/*     auto options = Modifier::getModifierOptions(); */
/**/
/*     int i = 0; */
/*     for (const auto& [type, info] : options) */
/*     { */
/*         // Draw the label (single letter) */
/*         juce::Rectangle<int> labelBounds( */
/*             20, */
/*             startY + (i * itemHeight), */
/*             labelWidth, */
/*             itemHeight */
/*         ); */
/**/
/*         // Draw the description (aligned at fixed position) */
/*         juce::Rectangle<int> descBounds( */
/*             descriptionX, */
/*             startY + (i * itemHeight), */
/*             getWidth() - descriptionX - 20, */
/*             itemHeight */
/*         ); */
/**/
/*         // Convert char to String for the shortcut */
/*         juce::String shortcut = juce::String::charToString(info.shortcut); */
/**/
/*         g.drawText(shortcut, */
/*                   labelBounds, */
/*                   juce::Justification::left, */
/*                   false); */
/**/
/*         g.drawText(info.displayName, */
/*                   descBounds, */
/*                   juce::Justification::left, */
/*                   false); */
/**/
/*         i++; */
/*     } */
/* } */

/* void ModifierMenuComponent::resized () */
/* { */
/**/
/* } */

/* bool ModifierMenuComponent::keyPressed(const juce::KeyPress& key) */
/* { */
/*     DBG("HI"); */
/*     return true; */
/**/
/* } */
/* { */
/**/
/*     if (mode == ModMenuMode::top) { */
/*         if (key.getTextCharacter() == 'a') */
/*         { */
/*             mode = ModMenuMode::add; */
/*             return true; */
/*         } */
/**/
/*         if (key.getTextCharacter() == 'e') */
/*         { */
/*             mode = ModMenuMode::edit; */
/*             return true; */
/*         } */
/**/
/*         if (key.getTextCharacter() == 'r') */
/*         { */
/*             mode = ModMenuMode::remove; */
/*             return true; */
/*         } */
/*     } */
/**/
/*     if (key.getTextCharacter() == 'r') */
/*     { */
/*         return handleModifierChange(ModifierType::randomTrigger); */
/*     } */
/**/
/*     if (key.getTextCharacter() == 'v') */
/*     { */
/*         return handleModifierChange(ModifierType::velocity); */
/*     } */
/**/
/*     if (key.getTextCharacter() == 'o') */
/*     { */
/*         return handleModifierChange(ModifierType::octave); */
/*     } */
/**/
/*     return false; */
/* } */

/* void ModifierMenuComponent::focusGained(FocusChangeType) */
/* { */
/*     mode = ModMenuMode::top; */
/* } */
/**/
/* void ModifierMenuComponent::timerCallback() */
/* { */
/*     stopTimer(); */
/*     mode = ModMenuMode::top; */
/*     completeMessage = ""; */
/* } */
/**/
/* void ModifierMenuComponent::showMessage(juce::String message) */
/* { */
/*     startTimer(3000); */
/*     mode = ModMenuMode::complete; */
/*     completeMessage = message; */
/* } */

bool ModifierMenuComponent::handleModifierChange (ModifierType t)
{
    if (mode == ModMenuMode::add)
    {
        int numModifiersAdded = cursor.addModifier (t);

        if (numModifiersAdded == 0)
        {
            completeMessage = "No notes selected";
        }
        else
        {
            auto* def = ModifierRegistry::getInstance().getDefinition (t);
            juce::String modName = def ? def->displayName : t.toString();
            completeMessage = "Added " + modName + " modifier to " + juce::String (numModifiersAdded) + " notes.";
        }
    }

    if (mode == ModMenuMode::remove)
    {
        int numModifiersAdded = cursor.removeModifier (t);

        if (numModifiersAdded == 0)
        {
            completeMessage = "No notes selected";
        }
        else
        {
            auto* defRemove = ModifierRegistry::getInstance().getDefinition (t);
            juce::String modNameRemove = defRemove ? defRemove->displayName : t.toString();
            completeMessage = "Removed " + modNameRemove + " modifier from " + juce::String (numModifiersAdded) + " notes.";
        }
    }

    mode = ModMenuMode::complete;
    startTimer (3000);
    return true;
}
