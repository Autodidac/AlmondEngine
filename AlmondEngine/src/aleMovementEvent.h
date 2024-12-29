
#pragma once

#include <iostream>

namespace almond {

class MovementEvent {
public:
    MovementEvent(int entityId, float deltaX, float deltaY)
        : entityId(entityId), deltaX(deltaX), deltaY(deltaY) {}

    void print() const {
        std::cout << "Movement Event - Entity ID: " << entityId
            << ", Amount: (" << deltaX << ", " << deltaY << ")\n";
    }

    int getEntityId() const { return entityId; }
    float getDeltaX() const { return deltaX; }
    float getDeltaY() const { return deltaY; }

private:
    int entityId; // ID of the entity to move
    float deltaX; // Change in X position
    float deltaY; // Change in Y position
};
} // namespace almond
