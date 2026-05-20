#include "ui/anim/Animation.h"

namespace pos::ui {

float Animation::easeOutQuad(float t) {
    if (t < 0.f) t = 0.f;
    if (t > 1.f) t = 1.f;
    return 1.f - (1.f - t) * (1.f - t);
}

float Animation::easeInOutCubic(float t) {
    if (t < 0.f) t = 0.f;
    if (t > 1.f) t = 1.f;
    if (t < 0.5f) return 4.f * t * t * t;
    float p = -2.f * t + 2.f;
    return 1.f - p * p * p / 2.f;
}

float Animation::lerp(float a, float b, float t) { return a + (b - a) * t; }

float Animation::clamp01(float t) { return t < 0.f ? 0.f : (t > 1.f ? 1.f : t); }

} // namespace pos::ui
