#pragma once

#include "imgui.h"

namespace pos::ui {

enum class ThemeMode { Dark, Light };

class Theme {
public:
    static void apply(ThemeMode mode);

    // Brand colors
    static constexpr ImU32 brandAccent  = IM_COL32(255, 107, 53, 255); // #FF6B35
    static constexpr ImU32 colorSuccess = IM_COL32( 34, 197,  94, 255);
    static constexpr ImU32 colorWarning = IM_COL32(245, 158,  11, 255);
    static constexpr ImU32 colorDanger  = IM_COL32(239,  68,  68, 255);
    static constexpr ImU32 colorInfo    = IM_COL32( 59, 130, 246, 255);
};

} // namespace pos::ui
