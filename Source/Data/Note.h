/*
  ==============================================================================

    Note.h
    Created: 17 Jan 2025 7:00:41am
    Author:  Danny Keig

  ==============================================================================
*/


#include <JuceHeader.h>
#pragma once

class Note
{
public:
    Note();
    Note(int deg);
    ~Note();
    int degree = 0;

    bool isSelected() const;
    void setIsSelectedCallback(std::function<bool(const Note&)> callback);

private:
    std::function<bool(const Note&)> isSelectedCallback = nullptr;

};
