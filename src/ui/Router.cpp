#include "ui/Router.h"

namespace pos::ui {

void Router::go(Screen s) {
    if (s == current_) return;
    history_.push_back(current_);
    current_ = s;
    if (history_.size() > 32) history_.erase(history_.begin());
}

void Router::back() {
    if (history_.empty()) return;
    current_ = history_.back();
    history_.pop_back();
}

} // namespace pos::ui
