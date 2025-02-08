
#include <JuceHeader.h>
#include "Components/StatusBarComponent.h"
#include "juce_core/system/juce_PlatformDefs.h"

//==============================================================================


StatusBarComponent::StatusBarComponent(const Cursor& c) : cursor(c)
{
    repaint();
}

StatusBarComponent::~StatusBarComponent()
{
}

void StatusBarComponent::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds();

    // Draw background
    g.setColour(juce::Colours::lightgrey);
    g.fillRect(bounds);

    // Draw border line at top
    g.setColour(juce::Colours::grey);
    g.drawHorizontalLine(0, 0.0f, static_cast<float>(bounds.getWidth()));

    // Draw text
    g.setColour(juce::Colours::black);
    g.setFont(14.0f);

    // Draw MODE

    // Calculate the width needed for the text
    const int leftPadding = 15;  // Pixels from left edge
    const int rightPadding = 15; // Padding after text
    g.setFont(14.0f);
    int textWidth = 50;

    if (cursor.isNormalMode()) g.setColour(juce::Colours::pink);
    if (cursor.isVisualBlockMode()) g.setColour(juce::Colours::lightcyan);
    if (cursor.isVisualLineMode()) g.setColour(juce::Colours::lightsalmon);
    if (cursor.isInsertMode()) g.setColour(juce::Colours::lightgreen);

    juce::Rectangle<int> textBackground(0, 0, leftPadding + textWidth + rightPadding, bounds.getHeight());
    g.fillRect(textBackground);

    // Draw light border around the text background
    g.setColour(juce::Colours::black.withAlpha(0.3f));  // Semi-transparent black
    g.drawRect(textBackground, 1);  // 1 pixel border width

    // Draw text
    g.setColour(juce::Colours::black);
    auto textBounds = bounds.withTrimmedLeft(leftPadding);
    g.drawText(cursor.getModeName(), textBounds,
              juce::Justification::centredLeft, true);

    // Draw pie chart rectangle
    const int pieSize = bounds.getHeight() - 8;  // Leave 4px padding top and bottom
    const int pieMargin = 8;  // Space between text background and pie
    juce::Rectangle<int> pieBackground(
        textBackground.getRight() + pieMargin,
        (bounds.getHeight() - pieSize) / 2,
        pieSize,
        pieSize
    );

    // Draw pie background rectangle
    g.setColour(juce::Colours::lightgrey.darker(0.1f));
    g.fillRect(pieBackground);
    g.setColour(juce::Colours::black.withAlpha(0.3f));
    g.drawRect(pieBackground, 1);

    // Draw pie chart
    auto pieRect = pieBackground.reduced(2).toFloat();  // Slight padding inside rectangle

    // Draw background circle (empty portion)
    g.setColour(juce::Colours::white);
    g.fillEllipse(pieRect);


    setPiePercentage(static_cast<float>(cursor.timeline.getStepSize()));

    // Draw filled portion
    g.setColour(juce::Colours::lightpink);

    float startAngle = 0.0f;
    float endAngle = startAngle + (piePercentage * juce::MathConstants<float>::twoPi);

    juce::Path piePath;
    piePath.addPieSegment(pieRect, startAngle, endAngle, 0.0f);
    g.fillPath(piePath);

    // Draw circle border
    g.setColour(juce::Colours::black.withAlpha(0.3f));
    g.drawEllipse(pieRect, 1.0f);

    // Draw percentage text in the middle
    g.setColour(juce::Colours::black);
    g.setFont(pieRect.getHeight() * 0.4f);  // Font size relative to circle size
}

void StatusBarComponent::resized()
{
    // Component will automatically resize width to match parent,
    // but maintain fixed height
    auto parent = getParentComponent();
    if (parent != nullptr)
    {
        setBounds(0, parent->getHeight() - barHeight,
                 parent->getWidth(), barHeight);
    }
}



void StatusBarComponent::setPiePercentage(float percentage)
{
    // Ensure percentage is between 0 and 1
    piePercentage = juce::jlimit(0.0f, 1.0f, percentage);
    repaint();
}
