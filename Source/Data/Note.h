/*
  ==============================================================================

    Note.h
    Created: 17 Jan 2025 7:00:41am
    Author:  Danny Keig

  ==============================================================================
*/


#include <JuceHeader.h>
#include "Data/Selectable.h"
#pragma once

class Note : public Selectable<Note>
{
public:
    using Selectable<Note>::Selectable;
    ~Note();
    int degree = 0;

private:

};
