#pragma once

#include "Scene.h"

#include <iostream>
#include <string>

namespace almond {

class Event {
public:
    // Change to accept a Scene pointer to avoid copying
    Event(const std::string& description, Scene& scene)
        : description(description), scene(scene) {}

    const std::string& getDescription() const { return description; }
    Scene& getScene() const { return scene; } // Change to return a reference

    void print() const {
        std::cout << "Event: " << description << ", ";
        scene.printEntityPositions();
    }

private:
    std::string description; // Description of the event
    Scene& scene; // Reference to the scene associated with the event
};
} // namespace almond
