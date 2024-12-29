#pragma once

#include "aleScene.h"

#include <chrono>
#include <memory>

namespace almond {

    struct SceneSnapshot {
        float timeStamp = 0;
        std::unique_ptr<Scene> currentState;

        SceneSnapshot() = default;

        SceneSnapshot(float ts, std::unique_ptr<Scene> state)
            : timeStamp(ts), currentState(std::move(state)) {}

        // Delete copy constructor and copy assignment operator
        SceneSnapshot(const SceneSnapshot&) = delete;
        SceneSnapshot& operator=(const SceneSnapshot&) = delete;

        // Enable move constructor and move assignment operator
        SceneSnapshot(SceneSnapshot&&) noexcept = default;
        SceneSnapshot& operator=(SceneSnapshot&&) noexcept = default;
    };

} // namespace almond
