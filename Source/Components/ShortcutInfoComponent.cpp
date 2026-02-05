#include "Components/ShortcutInfoComponent.h"
#include "juce_graphics/juce_graphics.h"

ShortcutInfoComponent::ShortcutInfoComponent (KeyboardShortcutManager& manager) : keyboardShortcutManager (manager)
{
    setWantsKeyboardFocus (true);
}

ShortcutInfoComponent::~ShortcutInfoComponent() {}

void ShortcutInfoComponent::resized()
{
}

void ShortcutInfoComponent::update() {}

void ShortcutInfoComponent::paint (juce::Graphics& g)
{
    // Calculate section heights: 70%, 10%, 10%, 10%
    auto totalHeight = getHeight();
    auto listSectionHeight = static_cast<int> (totalHeight * 0.7);
    auto infoSectionHeight = static_cast<int> (totalHeight * 0.1);
    auto pageSectionHeight = static_cast<int> (totalHeight * 0.1);
    auto navHelpSectionHeight = totalHeight - listSectionHeight - infoSectionHeight - pageSectionHeight;

    auto listSection = juce::Rectangle<int> (0, 0, getWidth(), listSectionHeight);
    auto infoSection = juce::Rectangle<int> (0, listSectionHeight, getWidth(), infoSectionHeight);
    auto pageSection = juce::Rectangle<int> (0, listSectionHeight + infoSectionHeight, getWidth(), pageSectionHeight);
    auto navHelpSection = juce::Rectangle<int> (0, listSectionHeight + infoSectionHeight + pageSectionHeight, getWidth(), navHelpSectionHeight);

    g.setColour (juce::Colours::aquamarine);

    // Draw sections
    drawShortcutList (g, listSection);
    drawSelectedInfo (g, infoSection);
    drawPageIndicators (g, pageSection);
    drawNavHelp (g, navHelpSection);
}

void ShortcutInfoComponent::drawShortcutList (juce::Graphics& g, juce::Rectangle<int> bounds)
{
    auto shortcuts = keyboardShortcutManager.getShortcutsForMode (Mode::normal);
    if (shortcuts.empty())
        return;

    auto itemHeight = bounds.getHeight() / numItemsPerPage;
    float padding = 20.0f;
    int keyColumnWidth = 100;

    for (size_t i = 0; i < shortcuts.size(); i++)
    {
        if (i >= static_cast<size_t> (currentPage * numItemsPerPage) && i < static_cast<size_t> ((currentPage + 1) * numItemsPerPage))
        {
            auto yIndex = static_cast<int> (i % static_cast<size_t> (numItemsPerPage));
            auto y = bounds.getY() + yIndex * itemHeight;

            // Draw L-shape highlight for selected item
            if (yIndex == selectedIndex)
            {
                g.setColour (juce::Colours::aquamarine);
                // Left edge
                g.drawLine (static_cast<float> (bounds.getX()) + padding / 2.0f,
                            static_cast<float> (y),
                            static_cast<float> (bounds.getX()) + padding / 2.0f,
                            static_cast<float> (y + itemHeight),
                            2.0f);
                // Bottom edge
                g.drawLine (static_cast<float> (bounds.getX()) + padding / 2.0f,
                            static_cast<float> (y + itemHeight),
                            static_cast<float> (bounds.getRight()) - padding / 2.0f,
                            static_cast<float> (y + itemHeight),
                            2.0f);
            }

            // Draw shortcut key
            g.setColour (juce::Colours::white);
            g.drawText (shortcuts[i].getShortcutKey(),
                        bounds.getX() + static_cast<int> (padding),
                        y,
                        keyColumnWidth,
                        itemHeight,
                        juce::Justification::centredLeft,
                        true);

            // Draw short description
            g.drawText (shortcuts[i].shortDescription,
                        bounds.getX() + static_cast<int> (padding) + keyColumnWidth,
                        y,
                        bounds.getWidth() - static_cast<int> (padding) - keyColumnWidth,
                        itemHeight,
                        juce::Justification::centredLeft,
                        true);
        }
    }
}

void ShortcutInfoComponent::drawSelectedInfo (juce::Graphics& g, juce::Rectangle<int> bounds)
{
    auto shortcuts = keyboardShortcutManager.getShortcutsForMode (Mode::normal);
    if (shortcuts.empty())
        return;

    auto globalIndex = static_cast<size_t> (currentPage * numItemsPerPage + selectedIndex);
    if (globalIndex >= shortcuts.size())
        return;

    g.setColour (juce::Colours::white);
    g.drawText ("Info: " + shortcuts[globalIndex].longDescription,
                bounds.reduced (10, 0), // Only reduce horizontally, not vertically
                juce::Justification::left,
                true);
}

void ShortcutInfoComponent::drawPageIndicators (juce::Graphics& g, juce::Rectangle<int> bounds)
{
    auto numPages = getNumPages();
    if (numPages <= 0)
        return;

    auto pageIndicatorPadding = 50;
    auto pageIndicatorGap = 10;
    auto pageIndicatorHeight = bounds.getHeight() / 2;

    auto totalPaddingSpace = (2 * pageIndicatorPadding) + ((numPages - 1) * pageIndicatorGap);
    auto pageIndicatorWidth = (bounds.getWidth() - totalPaddingSpace) / numPages;

    auto y = bounds.getY() + (bounds.getHeight() - pageIndicatorHeight) / 2;

    g.setColour (juce::Colours::aquamarine);

    for (int i = 0; i < numPages; i++)
    {
        auto x = bounds.getX() + pageIndicatorPadding + i * (pageIndicatorWidth + pageIndicatorGap);
        auto pageIndicatorRect = juce::Rectangle<int> (x, y, pageIndicatorWidth, pageIndicatorHeight);

        if (i == currentPage)
            g.fillRect (pageIndicatorRect);
        else
            g.drawRect (pageIndicatorRect, 2.0f);
    }
}

void ShortcutInfoComponent::drawNavHelp (juce::Graphics& g, juce::Rectangle<int> bounds)
{
    g.setColour (juce::Colours::lightgrey);
    g.setFont (juce::Font (juce::FontOptions (16.0f).withStyle ("Italic")));
    g.drawText (juce::String::fromUTF8 ("j/↓   k/↑: Select   h/←   l/→: Page   ESC: Back"),
                bounds.reduced (10, 0),
                juce::Justification::centred,
                true);
}

bool ShortcutInfoComponent::keyPressed (const juce::KeyPress& key)
{
    // Page navigation: l or Right arrow
    if (key == juce::KeyPress ('l') || key == juce::KeyPress::rightKey)
    {
        nextPage();
        selectedIndex = 0;
        repaint();
        return true;
    }
    // Page navigation: h or Left arrow
    if (key == juce::KeyPress ('h') || key == juce::KeyPress::leftKey)
    {
        prevPage();
        selectedIndex = 0;
        repaint();
        return true;
    }
    // Item navigation: j or Down arrow
    if (key == juce::KeyPress ('j') || key == juce::KeyPress::downKey)
    {
        selectNext();
        return true;
    }
    // Item navigation: k or Up arrow
    if (key == juce::KeyPress ('k') || key == juce::KeyPress::upKey)
    {
        selectPrev();
        return true;
    }
    return false;
}

void ShortcutInfoComponent::nextPage()
{
    auto numPages = getNumPages();
    if (numPages == 0)
        return;

    currentPage = (currentPage + 1) % numPages;
}

void ShortcutInfoComponent::prevPage()
{
    auto numPages = getNumPages();
    if (numPages == 0)
        return;

    currentPage = (currentPage + numPages - 1) % numPages;
}

void ShortcutInfoComponent::selectNext()
{
    auto shortcuts = keyboardShortcutManager.getShortcutsForMode (Mode::normal);
    if (shortcuts.empty())
        return;

    auto totalShortcuts = static_cast<int> (shortcuts.size());
    auto itemsOnCurrentPage = std::min (numItemsPerPage, totalShortcuts - (currentPage * numItemsPerPage));

    if (itemsOnCurrentPage <= 0)
        return;

    selectedIndex = (selectedIndex + 1) % itemsOnCurrentPage;
    repaint();
}

void ShortcutInfoComponent::selectPrev()
{
    auto shortcuts = keyboardShortcutManager.getShortcutsForMode (Mode::normal);
    if (shortcuts.empty())
        return;

    auto totalShortcuts = static_cast<int> (shortcuts.size());
    auto itemsOnCurrentPage = std::min (numItemsPerPage, totalShortcuts - (currentPage * numItemsPerPage));

    if (itemsOnCurrentPage <= 0)
        return;

    selectedIndex = (selectedIndex + itemsOnCurrentPage - 1) % itemsOnCurrentPage;
    repaint();
}

int ShortcutInfoComponent::getNumPages()
{
    auto numShortcuts = keyboardShortcutManager.getShortcutsForMode (Mode::normal).size();
    auto numPages = numShortcuts / static_cast<size_t> (numItemsPerPage);
    if (numShortcuts % static_cast<size_t> (numItemsPerPage) != 0)
        numPages++;
    return static_cast<int> (numPages);
}

int ShortcutInfoComponent::getNumShortcuts()
{
    return static_cast<int> (keyboardShortcutManager.getShortcutsForMode (Mode::normal).size());
}
