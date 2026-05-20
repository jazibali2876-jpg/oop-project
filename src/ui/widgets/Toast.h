#pragma once

#include <string>

namespace pos::ui {

enum class ToastKind { Info, Success, Warning, Danger };

struct Toast {
    std::string title;
    std::string body;
    ToastKind   kind = ToastKind::Info;
    float       ttl  = 4.0f;   // seconds remaining
    float       age  = 0.f;
};

} // namespace pos::ui
