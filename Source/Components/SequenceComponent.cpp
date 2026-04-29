/*
  ==============================================================================

    SequenceComponent.cpp
    Created: 13 Jan 2025 3:17:50pm
    Author:  Danny Keig

  ==============================================================================
*/

#include "SequenceComponent.h"
#include "AppColours.h"
#include "Audio/Transport.h"
#include "CoordinateUtils.h"
#include <JuceHeader.h>
#include <unordered_set>

//==============================================================================
SequenceComponent::SequenceComponent (const Cursor& c, const Transport& t)
    : cursor (c), transport (t)
{
    setWantsKeyboardFocus (false);
}

SequenceComponent::~SequenceComponent()
{
}

void SequenceComponent::paint (juce::Graphics& g)
{
    float width = static_cast<float> (getWidth());
    float height = static_cast<float> (getHeight());

    // Draw the playhead
    g.setColour (AppColours::playhead);

    const auto& selectedSequence = cursor.getSelectedSequence();
    double tempoBPM = transport.getTempo();

    double sequenceDurationInSeconds = selectedSequence.getLengthSeconds (tempoBPM);

    double loopedPosition = std::fmod (currentPlayheadTime_, sequenceDurationInSeconds);
    double playheadX = (loopedPosition / sequenceDurationInSeconds) * width;
    g.drawLine (static_cast<float> (playheadX), 0, static_cast<float> (playheadX), height, 3.0f);

    // Draw the outline
    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);

    // Collect notes that should show duration lines:
    // - in normal/insert mode: notes at the cursor point
    // - in visual modes: all notes in the selection
    std::unordered_set<Note*> durationLineSet;
    if (cursor.isVisualBlockMode() || cursor.isVisualLineMode())
    {
        for (auto& ref : cursor.findNotesInCursorSelection())
            durationLineSet.insert (ref.get().get());
    }
    else
    {
        for (auto& ref : cursor.findNotesAtCursor())
            durationLineSet.insert (ref.get().get());
    }

    // Draw the notes
    for (auto& note : cursor.getSelectedSequence().notes)
    {
        auto tri = CoordinateUtils::getTriangleAtPoint (*note, width, height, cursor.getCurrentTimeline(), cursor.getCurrentScale());

        // Duration line — only for the note under the cursor
        // Drawn first so the triangle paints on top of it
        // Anchored at the left edge of the triangle (note start), extending right
        if (durationLineSet.count (note.get()) > 0)
        {
            auto origin = CoordinateUtils::musicToScreen (*note, width, height, cursor.getCurrentTimeline(), cursor.getCurrentScale());
            float stepHeight = CoordinateUtils::getStepHeight (height);
            float midY = origin.y + stepHeight / 2.0f;
            float durWidth = CoordinateUtils::getDurationPixelWidth (*note, width, cursor.getCurrentTimeline());
            g.setColour (juce::Colours::darkgrey);
            g.drawLine (origin.x, midY, origin.x + durWidth, midY, 1.5f);
        }

        // Velocity shading: map 0-127 to darkening amount, clamped so even
        // velocity-0 notes remain visibly coloured (max darkening = 0.5)
        float velocityT = static_cast<float> (note->getVelocity()) / 127.0f;
        float darkenAmount = 0.5f * (1.0f - velocityT);

        juce::Colour baseColour = note->hasAnyModifier() ? juce::Colours::orange
                                                         : juce::Colours::lightgrey;
        // Duration fill and ghost notes: only show while note is actually playing
        if (transport.isPlaying() && note->lastTriggeredMidiNote.has_value())
        {
            const auto& triggered = *note->lastTriggeredMidiNote;
            // Keep timing calculations in double precision to avoid precision loss
            double noteStart = triggered.startTime;
            double noteDur = triggered.duration;
            double elapsed = loopedPosition - noteStart;

            // Add timing tolerance to account for frame timing and floating-point precision
            // Use a small tolerance (1/60th second ~= 16.67ms) to handle UI/audio sync differences
            const double timingTolerance = 1.0 / 60.0;

            // Check if note is currently playing (within its duration window with tolerance)
            bool isCurrentlyPlaying = (elapsed >= -timingTolerance && elapsed <= (noteDur + timingTolerance));

            // Only draw ghost notes while the note is playing
            if (! triggered.isMuted && isCurrentlyPlaying && triggered.noteNumber != (selectedSequence.getRootNote() + static_cast<int> (note->getDegree())))
            {
                // Original left edge position
                auto origPoint = CoordinateUtils::musicToScreen (*note, width, height, cursor.getCurrentTimeline(), cursor.getCurrentScale());
                float stepW = CoordinateUtils::getStepWidthAtSmallestSize (width, cursor.getCurrentTimeline());
                float stepH = CoordinateUtils::getStepHeight (height);
                float origLeftX = origPoint.x;
                float origMidY = origPoint.y + stepH * 0.5f;

                double ghostDegree = static_cast<double> (triggered.noteNumber - selectedSequence.getRootNote());
                auto ghostPoint = CoordinateUtils::musicToScreen (note->getStartTime(), ghostDegree, width, height, cursor.getCurrentTimeline(), cursor.getCurrentScale());
                float ghostLeftX = ghostPoint.x;
                float ghostMidY = ghostPoint.y + stepH * 0.5f;

                // Draw connector from left edges
                g.setColour (juce::Colours::black);
                g.drawLine (origLeftX, origMidY, ghostLeftX, ghostMidY, 1.0f);

                // Build ghost triangle path
                juce::Path ghostTri;
                ghostTri.startNewSubPath (ghostPoint);
                ghostTri.lineTo (juce::Point<float> (ghostPoint.x + stepW, ghostPoint.y + stepH / 2.0f));
                ghostTri.lineTo (juce::Point<float> (ghostPoint.x, ghostPoint.y + stepH));
                ghostTri.closeSubPath();

                // Light red fill and subtle stroke
                g.setColour (juce::Colours::red.withAlpha (0.35f));
                g.fillPath (ghostTri);
                g.setColour (juce::Colours::black.withAlpha (0.6f));
                g.strokePath (ghostTri, juce::PathStrokeType (1.0f));
            }
        }

        g.setColour (baseColour.darker (darkenAmount));
        g.fillPath (tri);

        // Velocity flash: cyan overlay fading out over the note's duration
        // Do not show a velocity flash for muted notes
        if (transport.isPlaying() && note->lastTriggeredMidiNote.has_value())
        {
            const auto& triggered = *note->lastTriggeredMidiNote;
            float noteStart = static_cast<float> (triggered.startTime);
            float noteDur = static_cast<float> (triggered.duration);
            float elapsed = static_cast<float> (loopedPosition) - noteStart;

            if (! triggered.isMuted && elapsed >= 0.0f && elapsed < noteDur)
            {
                float fadeAlpha = 1.0f - (elapsed / noteDur);
                float triggeredV = static_cast<float> (triggered.velocity) / 127.0f;
                g.setColour (AppColours::velocityFlash.withAlpha (fadeAlpha * triggeredV));
                g.fillPath (tri);
            }
        }

        g.setColour (juce::Colours::black);
        g.strokePath (tri, juce::PathStrokeType (1.0f));
    }
}

void SequenceComponent::resized()
{
}

//==============================================================================
void SequenceComponent::update()
{
}

void SequenceComponent::setCurrentPlayheadTime (double time)
{
    currentPlayheadTime_ = time;
    repaint();
}
