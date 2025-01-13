/*
  ==============================================================================

    Sequence.cpp
    Created: 13 Jan 2025 3:17:18pm
    Author:  Danny Keig

  ==============================================================================
*/

#include "Sequence.h"

Sequence::Sequence()
{
    for (int i = 0; i < lengthBeats; ++i) {
        auto s = std::make_unique<Step>();
        steps.emplace_back(std::move(s));
    }
};

void Sequence::setIsSelectedCallback(std::function<bool(const Step&)> callback) {
    for (auto& step : steps) {
        step->setIsSelectedCallback(callback);
    }
}
