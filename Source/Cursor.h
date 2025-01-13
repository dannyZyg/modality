/*
  ==============================================================================

    Cursor.h
    Created: 13 Jan 2025 3:09:08pm
    Author:  Danny Keig

  ==============================================================================
*/

# include "Step.h"
#pragma once

class Cursor
{
    
public:
    Cursor();
    ~Cursor();
    
    void setVisualSelection();
private:
    std::vector<std::unique_ptr<Step>> visualSelection{};
    std::unique_ptr<Step> focusedStep = nullptr;
};
