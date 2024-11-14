#pragma once

#include "ComponentManager.h"
#include "JobSystem.h"

namespace almond {
class RenderingSystem {
public:
    RenderingSystem(ComponentManager& componentManager, JobSystem& jobSystem);
    void render();

private:
    ComponentManager& componentManager;
    JobSystem& jobSystem;
};

inline RenderingSystem::RenderingSystem(ComponentManager& componentManager, JobSystem& jobSystem)
    : componentManager(componentManager), jobSystem(jobSystem) {}

inline void RenderingSystem::render() {
    // Render graphics
    // Implement rendering logic here
}
} // namespace almond
