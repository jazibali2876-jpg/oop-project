#pragma once

#include "imgui.h"

#include <string>
#include <vector>

namespace pos::ui::widgets {

struct BarDatum   { std::string label; float value; ImU32 color = 0; };
struct PieSlice   { std::string label; float value; ImU32 color = 0; };

// All three charts are drawn manually using ImDrawList primitives (no ImPlot).
void BarChart (const std::vector<BarDatum>& data, ImVec2 size);
void LineChart(const std::vector<float>& series, ImVec2 size, ImU32 color = 0);
void PieChart (const std::vector<PieSlice>& slices, float radius);

} // namespace pos::ui::widgets
