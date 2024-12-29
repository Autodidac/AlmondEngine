#pragma once

namespace almond {
struct PositionComponent {
    float x, y;

    PositionComponent(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}
};

struct VelocityComponent {
    float vx, vy;

    VelocityComponent(float vx = 0.0f, float vy = 0.0f) : vx(vx), vy(vy) {}
};
} // namespace almond
