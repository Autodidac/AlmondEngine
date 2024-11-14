#pragma once

#include "Entity.h"
#include "ComponentManager.h"

#include <vector>
#include <iostream>

namespace almond {
class EntityComponentSystem {
public:
    // Create an Entity with a unique ID and default position (0.0f, 0.0f)
    int createEntity() {
        Entity entity(nextEntity++, 0.0f, 0.0f); // Ensure you pass parameters here
        entities.push_back(entity);
        std::cout << "Entity created with ID: " << entity.getId() << std::endl;
        return entity.getId(); // Return the ID of the created entity
    }

    template<typename T>
    void addComponent(int entityId, T component) {
        Entity& entity = getEntity(entityId);
        if (&entity != nullptr) {
            componentManager.addComponent<T>(entity, component);
        }
        else {
            std::cerr << "Entity with ID " << entityId << " does not exist!" << std::endl;
        }
    }

    template<typename T>
    T& getComponent(int entityId) {
        Entity& entity = getEntity(entityId);
        if (&entity != nullptr) {
            return componentManager.getComponent<T>(entity);
        }
        else {
            throw std::runtime_error("Entity with ID " + std::to_string(entityId) + " does not exist!");
        }
    }

private:
    int nextEntity = 0; // Use int to track the next entity ID
    std::vector<Entity> entities; // Store the entities
    ComponentManager componentManager; // Manage components

    Entity& getEntity(int entityId) {
        for (auto& entity : entities) {
            if (entity.getId() == entityId) {
                return entity;
            }
        }
        throw std::runtime_error("Entity not found");
    }
};
} // namespace almond
