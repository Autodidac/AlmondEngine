#pragma once

#include "ComponentManager.h"
#include "JobSystem.h"

namespace almond {
class PhysicsSystem {
public:
    PhysicsSystem(ComponentManager& componentManager, JobSystem& jobSystem);
    void update(float deltaTime);

private:
    ComponentManager& componentManager;
    JobSystem& jobSystem;
};

inline PhysicsSystem::PhysicsSystem(ComponentManager& componentManager, JobSystem& jobSystem)
    : componentManager(componentManager), jobSystem(jobSystem) {}

inline void PhysicsSystem::update(float deltaTime) {
    // Update physics components
    // Implement physics calculations and updates
}
} // namespace almond
