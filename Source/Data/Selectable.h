#pragma once

#include <JuceHeader.h>


template <typename T>
class Selectable {
protected:
    std::function<bool(const T&)> isSelectedCallback;

public:
    // Delete default constructor - We want to force the callback to be set on object creation
    // otherwise objects could find themselves in a state with no callback to check if they are selected!
    //Selectable() = delete;

    // Constructor with callback
    explicit Selectable(std::function<bool(const T&)> callback)
        : isSelectedCallback(std::move(callback)) {}

    bool isSelected() const {
        return isSelectedCallback ? isSelectedCallback(*static_cast<const T*>(this)) : false;
    }

    void setIsSelectedCallback(std::function<bool(const T&)> callback) {
        isSelectedCallback = callback;
    }
};
