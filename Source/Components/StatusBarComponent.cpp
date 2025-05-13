#include "Components/StatusBarComponent.h"
#include "AppColours.h"
#include <JuceHeader.h>

//==============================================================================

StatusBarComponent::StatusBarComponent (const Cursor& c) : cursor (c)
{
    setWantsKeyboardFocus (false);
    repaint();
}

StatusBarComponent::~StatusBarComponent()
{
}

void StatusBarComponent::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    const int padding = 20;
    const int textWidth = 50;
    const int height = bounds.getHeight();

    // Draw background
    g.setColour (juce::Colours::lightgrey);
    g.fillRect (bounds);

    // Draw border line at top
    g.setColour (juce::Colours::grey);
    g.drawHorizontalLine (0, 0.0f, static_cast<float> (bounds.getWidth()));

    g.setColour (juce::Colours::black);
    g.setFont (juce::Font (juce::Font::getDefaultMonospacedFontName(), 14.0f, juce::Font::plain));

    // Mode section (left side)
    juce::Rectangle<int> modeBox (0, 0, padding * 2 + textWidth, height);
    g.setColour (AppColours::getCursorColour (cursor.getMode()));
    g.fillRect (modeBox);
    g.setColour (juce::Colours::black.withAlpha (0.3f));
    g.drawRect (modeBox, 1);
    g.setColour (juce::Colours::black);
    g.drawText (cursor.getModeName(), modeBox.withTrimmedLeft (padding), juce::Justification::centredLeft, true);

    // Cursor position section (right side)
    juce::Rectangle<int> cursorBox (getWidth() - (padding * 2 + textWidth), 0, padding * 2 + textWidth, height);
    g.setColour (AppColours::getCursorColour (cursor.getMode()));
    g.fillRect (cursorBox);
    g.setColour (juce::Colours::black.withAlpha (0.3f));
    g.drawRect (cursorBox, 1);
    g.setColour (juce::Colours::black);
    g.drawText (cursor.readableCursorPosition(), cursorBox, juce::Justification::centred);

    // Draw pie chart rectangle
    const int pieSize = bounds.getHeight() - 8; // Leave 4px padding top and bottom
    const int pieMargin = 8; // Space between text background and pie
    juce::Rectangle<int> pieBackground (
        modeBox.getRight() + pieMargin,
        (bounds.getHeight() - pieSize) / 2,
        pieSize,
        pieSize);

    // Draw pie background rectangle
    g.setColour (juce::Colours::lightgrey.darker (0.1f));
    g.fillRect (pieBackground);
    g.setColour (juce::Colours::black.withAlpha (0.3f));
    g.drawRect (pieBackground, 1);

    // Draw pie chart
    auto pieRect = pieBackground.reduced (2).toFloat(); // Slight padding inside rectangle

    // Draw background circle (empty portion)
    g.setColour (juce::Colours::white);
    g.fillEllipse (pieRect);

    setPiePercentage (static_cast<float> (cursor.getCurrentTimeline().getStepSize()));

    // Draw filled portion
    g.setColour (AppColours::getCursorColour (cursor.getMode()));

    float startAngle = 0.0f;
    float endAngle = startAngle + (piePercentage * juce::MathConstants<float>::twoPi);

    juce::Path piePath;
    piePath.addPieSegment (pieRect, startAngle, endAngle, 0.0f);
    g.fillPath (piePath);

    // Draw circle border
    g.setColour (juce::Colours::black.withAlpha (0.3f));
    g.drawEllipse (pieRect, 1.0f);

    // After drawing mode text, add play icon
    const float iconSize = static_cast<float> (height) * 0.6f;

    // Create a triangle path for play icon
    juce::Path playPath;
    playPath.startNewSubPath (0, 0);
    playPath.lineTo (iconSize, iconSize / 2);
    playPath.lineTo (0, iconSize);
    playPath.closeSubPath();

    // Position it after the mode text
    g.setColour (juce::Colours::lightseagreen);
    auto playBounds = juce::Rectangle<float> (
        pieRect.getRight() + iconSize,
        (static_cast<float> (height) - iconSize) / 2, // Center vertically
        iconSize,
        iconSize);
    g.fillPath (playPath, playPath.getTransformToScaleToFit (playBounds, true));
}

void StatusBarComponent::resized()
{
    // Component will automatically resize width to match parent,
    // but maintain fixed height
    auto parent = getParentComponent();
    if (parent != nullptr)
    {
        setBounds (0, parent->getHeight() - barHeight, parent->getWidth(), barHeight);
    }
}

void StatusBarComponent::setPiePercentage (float percentage)
{
    // Ensure percentage is between 0 and 1
    piePercentage = juce::jlimit (0.0f, 1.0f, percentage);
    repaint();
}
