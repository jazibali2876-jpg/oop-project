#pragma once

#include "imgui.h"

namespace pos::ui::widgets {

// A simple metric card. `accent` colors the heading; size auto fits.
void Card(const char* title, const char* value, const char* sub, ImU32 accent, ImVec2 size);

} // namespace pos::ui::widgets
