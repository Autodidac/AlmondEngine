#pragma once

#include "aleEntity.h"
#include "aleComponentManager.h"

#include <vector>
#include <iostream>
#include <optional>

namespace almond {
    class EntityComponentSystem {
    public:
        // Create an Entity with a unique ID and default position (0.0f, 0.0f)
        int createEntity() {
            almond::Entity entity(nextEntity++, 0.0f, 0.0f, "entity.log"); // Pass parameters to Entity constructor
            entities.push_back(std::move(entity));  // Move the entity into the vector for better performance
            std::cout << "Entity created with ID: " << entity.getId() << std::endl;
            return entity.getId(); // Return the ID of the created entity
        }

        template<typename T>
        void addComponent(int entityId, T component) {
            try {
                almond::Entity& entity = getEntity(entityId);
                componentManager.addComponent<T>(entity, std::move(component)); // Move the component to optimize
            }
            catch (const std::runtime_error& e) {
                std::cerr << e.what() << std::endl;
            }
        }

        template<typename T>
        std::optional<T> getComponent(int entityId) {
            try {
                almond::Entity& entity = getEntity(entityId);
                return componentManager.getComponent<T>(entity); // Return the component if found
            }
            catch (const std::runtime_error& e) {
                std::cerr << e.what() << std::endl;
                return std::nullopt; // Return nullopt if not found
            }
        }

    private:
        int nextEntity = 0; // Use int to track the next entity ID
        std::vector<almond::Entity> entities; // Store the entities
        ComponentManager componentManager; // Manage components

        almond::Entity& getEntity(int entityId) {
            for (auto& entity : entities) {
                if (entity.getId() == entityId) {
                    return entity;
                }
            }
            throw std::runtime_error("Entity with ID " + std::to_string(entityId) + " not found");
        }
    };
} // namespace almond
