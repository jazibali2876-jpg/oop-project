#pragma once

#include <vector>

namespace pos::ui {

enum class Screen {
    Splash,
    Login,
    Dashboard,
    Menu,
    Order,
    Billing,
    Inventory,
    Tables,
    Kitchen,
    Analytics
};

class Router {
public:
    Router() = default;

    Screen current() const { return current_; }
    void   go(Screen s);
    void   back();

    bool   inAuthArea() const { return current_ == Screen::Splash || current_ == Screen::Login; }

private:
    Screen              current_ = Screen::Splash;
    std::vector<Screen> history_;
};

} // namespace pos::ui
