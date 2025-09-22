/**************************************************************
 *   █████╗ ██╗     ███╗   ███╗   ███╗   ██╗    ██╗██████╗    *
 *  ██╔══██╗██║     ████╗ ████║ ██╔═══██╗████╗  ██║██╔══██╗   *
 *  ███████║██║     ██╔████╔██║ ██║   ██║██╔██╗ ██║██║  ██║   *
 *  ██╔══██║██║     ██║╚██╔╝██║ ██║   ██║██║╚██╗██║██║  ██║   *
 *  ██║  ██║███████╗██║ ╚═╝ ██║ ╚██████╔╝██║ ╚████║██████╔╝   *
 *  ╚═╝  ╚═╝╚══════╝╚═╝     ╚═╝  ╚═════╝ ╚═╝  ╚═══╝╚═════╝    *
 *                                                            *
 *   This file is part of the Almond Project.                 *
 *   AlmondEngine - Modular C++ Game Engine                   *
 *                                                            *
 *   SPDX-License-Identifier: LicenseRef-MIT-NoSell           *
 *                                                            *
 *   Provided "AS IS", without warranty of any kind.          *
 *   Use permitted for non-commercial purposes only           *
 *   without prior commercial licensing agreement.            *
 *                                                            *
 *   Redistribution allowed with this notice.                 *
 *   No obligation to disclose modifications.                 *
 *   See LICENSE file for full terms.                         *
 **************************************************************/
#pragma once

#include "aplatform.hpp"   // must always come first  

#include "aentity.hpp"
#include "amovementevent.hpp" // Ensure you include this for MovementEvent
//#include <aecs.hpp>

#include <iostream>
#include <vector>
#include <memory> // Include for std::unique_ptr

namespace almondnamespace
{
    class Scene {
    public:
        Scene() = default;

        // Delete copy constructor and assignment operator
        Scene(const Scene&) = delete;
        Scene& operator=(const Scene&) = delete;

        // Allow move constructor and assignment operator
        Scene(Scene&&) noexcept = default;
        Scene& operator=(Scene&&) noexcept = default;

        virtual void load() {
            std::cout << "Scene loaded.\n";
            loaded = true; // Assuming you set a flag when loading
        }

        virtual void unload() {
            std::cout << "Scene unloaded.\n";
            loaded = false; // Reset the flag when unloading
        }

        virtual void printEntityPositions() const {
            for (const auto& entity : entities) {
                entity->printPosition(); // Print position of each entity
            }
        }

        void applyMovementEvent(const MovementEvent& event) {
            for (auto& entity : entities) {
                if (entity->getId() == event.getEntityId()) {
                    entity->move(event.getDeltaX(), event.getDeltaY());
                }
            }
        }

        void addEntity(std::unique_ptr<almondnamespace::ecs::Entity> entity) { // Accept unique_ptr
            entities.emplace_back(std::move(entity)); // Use std::move to transfer ownership
        }

        void clearEntities() {
            entities.clear(); // Clears the vector of entities
        }

        almondnamespace::ecs::Entity* getEntityById(int id) {
            for (const auto& entity : entities) {
                if (entity->getId() == id) {
                    return entity.get(); // Return raw pointer
                }
            }
            return nullptr; // Entity not found
        }

        // Clone method to create a copy of the scene
        std::unique_ptr<Scene> clone() const {
            auto newScene = std::make_unique<Scene>();

            for (const auto& entity : entities) {
                if (entity) { // Check if the entity is not null
                    newScene->addEntity(entity->clone()); // Assuming `clone()` returns a `std::unique_ptr<Entity>`
                }
            }
            return newScene;
        }

        bool isLoaded() const { return loaded; } // Check if the scene is loaded

    private:
        std::vector<std::unique_ptr<almondnamespace::ecs::Entity>> entities; // Store entities as pointers
        bool loaded = false; // Flag to indicate if the scene is loaded
    };

} // namespace almond
