#pragma once

#include "ui/widgets/Toast.h"

#include <deque>
#include <string>

namespace pos::ui {

class ToastQueue {
public:
    void info   (std::string title, std::string body = "");
    void success(std::string title, std::string body = "");
    void warn   (std::string title, std::string body = "");
    void danger (std::string title, std::string body = "");

    // Per-frame: decrement TTLs, prune expired, then draw current toasts top-right.
    void tickAndDraw(float dt, float screenW);

private:
    void push(Toast t);
    std::deque<Toast> queue_;
};

} // namespace pos::ui
