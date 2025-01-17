/*
  ==============================================================================

    Note.cpp
    Created: 17 Jan 2025 7:00:41am
    Author:  Danny Keig

  ==============================================================================
*/

#include "Note.h"


Note::Note()
{
    
}

Note::Note(int deg)
{
    degree = deg;
}

Note::~Note()
{
    
}

bool Note::isSelected() const {
    return isSelectedCallback ? isSelectedCallback(*this) : false;
}

void Note::setIsSelectedCallback(std::function<bool (const Note &)> callback)
{
    isSelectedCallback = callback;
}
