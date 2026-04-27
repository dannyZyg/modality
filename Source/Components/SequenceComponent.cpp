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
        g.setColour (baseColour.darker (darkenAmount));
        g.fillPath (tri);

        // Velocity flash: cyan overlay fading out over the note's duration
        // Uses the post-modifier velocity from the last scheduling pass
        // Guarded on transport playing to avoid stale flash when stopped
        if (transport.isPlaying() && note->lastTriggeredMidiNote.has_value())
        {
            const auto& triggered = *note->lastTriggeredMidiNote;
            float noteStart = static_cast<float> (triggered.startTime);
            float noteDur = static_cast<float> (triggered.duration);
            float elapsed = static_cast<float> (loopedPosition) - noteStart;

            if (elapsed >= 0.0f && elapsed < noteDur)
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
