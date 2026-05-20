#pragma once

namespace pos::ui {

class Animation {
public:
    static float easeOutQuad(float t);
    static float easeInOutCubic(float t);
    static float lerp(float a, float b, float t);
    static float clamp01(float t);
};

struct Tween {
    float current = 0.f;
    float target  = 0.f;
    float speed   = 8.f;   // per second

    void step(float dt) {
        float k = 1.f - 1.f / (1.f + speed * dt);
        current += (target - current) * k;
    }
};

} // namespace pos::ui
