#pragma once

#include "imgui.h"

#include <string>

namespace pos::ui {

class Fonts {
public:
    static bool load(ImGuiIO& io, const std::string& fontDir);

    static ImFont* regular() { return regular_; }
    static ImFont* bold()    { return bold_; }
    static ImFont* large()   { return large_; }

private:
    inline static ImFont* regular_ = nullptr;
    inline static ImFont* bold_    = nullptr;
    inline static ImFont* large_   = nullptr;
};

} // namespace pos::ui
