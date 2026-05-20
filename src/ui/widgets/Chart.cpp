#include "ui/widgets/Chart.h"
#include "ui/theme/Theme.h"

#include <algorithm>
#include <cmath>

namespace pos::ui::widgets {

static const ImU32 kPalette[] = {
    IM_COL32(255, 107,  53, 255),
    IM_COL32( 59, 130, 246, 255),
    IM_COL32( 34, 197,  94, 255),
    IM_COL32(245, 158,  11, 255),
    IM_COL32(168,  85, 247, 255),
    IM_COL32(236,  72, 153, 255),
    IM_COL32( 14, 165, 233, 255),
};

void BarChart(const std::vector<BarDatum>& data, ImVec2 size) {
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 origin = ImGui::GetCursorScreenPos();
    ImGui::Dummy(size);

    if (data.empty()) return;
    float maxVal = 0.f;
    for (const auto& d : data) maxVal = std::max(maxVal, d.value);
    if (maxVal <= 0.f) maxVal = 1.f;

    const float pad      = 8.f;
    const float bottomH  = 22.f;
    float chartLeft   = origin.x + pad;
    float chartTop    = origin.y + pad;
    float chartRight  = origin.x + size.x - pad;
    float chartBottom = origin.y + size.y - bottomH;
    float chartH      = chartBottom - chartTop;
    float n           = (float)data.size();
    float slotW       = (chartRight - chartLeft) / n;
    float barW        = slotW * 0.62f;

    // Baseline
    dl->AddLine(ImVec2(chartLeft, chartBottom), ImVec2(chartRight, chartBottom),
                IM_COL32(120, 130, 140, 100));

    for (std::size_t i = 0; i < data.size(); ++i) {
        const auto& d = data[i];
        float ratio = d.value / maxVal;
        float h = ratio * chartH;
        float cx = chartLeft + i * slotW + slotW * 0.5f;
        ImU32 col = d.color ? d.color : kPalette[i % (sizeof(kPalette) / sizeof(kPalette[0]))];
        ImVec2 a(cx - barW * 0.5f, chartBottom - h);
        ImVec2 b(cx + barW * 0.5f, chartBottom);
        dl->AddRectFilled(a, b, col, 3.f);

        // Label
        ImVec2 lblSize = ImGui::CalcTextSize(d.label.c_str());
        dl->AddText(ImVec2(cx - lblSize.x * 0.5f, chartBottom + 2),
                    IM_COL32(180, 185, 195, 230), d.label.c_str());
    }
}

void LineChart(const std::vector<float>& series, ImVec2 size, ImU32 color) {
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 origin = ImGui::GetCursorScreenPos();
    ImGui::Dummy(size);

    if (series.size() < 2) return;

    float lo = series[0], hi = series[0];
    for (float v : series) { lo = std::min(lo, v); hi = std::max(hi, v); }
    if (hi <= lo) hi = lo + 1.f;

    const float pad = 8.f;
    float left   = origin.x + pad;
    float top    = origin.y + pad;
    float right  = origin.x + size.x - pad;
    float bottom = origin.y + size.y - pad;
    float w      = right - left;
    float h      = bottom - top;

    ImU32 c = color ? color : Theme::brandAccent;

    // Baseline grid
    for (int i = 0; i < 4; ++i) {
        float y = top + h * (i / 4.f);
        dl->AddLine(ImVec2(left, y), ImVec2(right, y), IM_COL32(120, 130, 140, 50));
    }

    float n = (float)series.size() - 1.f;
    ImVec2 prev;
    for (std::size_t i = 0; i < series.size(); ++i) {
        float t  = (n > 0) ? (i / n) : 0.f;
        float ny = (series[i] - lo) / (hi - lo);
        ImVec2 pt(left + t * w, bottom - ny * h);
        if (i > 0) {
            dl->AddLine(prev, pt, c, 2.5f);
        }
        prev = pt;
    }
    // Dots
    for (std::size_t i = 0; i < series.size(); ++i) {
        float t  = (n > 0) ? (i / n) : 0.f;
        float ny = (series[i] - lo) / (hi - lo);
        ImVec2 pt(left + t * w, bottom - ny * h);
        dl->AddCircleFilled(pt, 3.f, c);
    }
}

void PieChart(const std::vector<PieSlice>& slices, float radius) {
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 origin = ImGui::GetCursorScreenPos();
    ImVec2 center(origin.x + radius + 10, origin.y + radius + 10);
    ImGui::Dummy(ImVec2(radius * 2 + 20, radius * 2 + 20));

    float total = 0.f;
    for (const auto& s : slices) total += s.value;
    if (total <= 0.f) return;

    const int segPerSlice = 48;
    float startAngle = -1.5707963f; // -90 deg = top
    for (std::size_t i = 0; i < slices.size(); ++i) {
        const auto& s = slices[i];
        float frac = s.value / total;
        float endAngle = startAngle + frac * 6.2831853f;

        ImU32 col = s.color ? s.color : kPalette[i % (sizeof(kPalette) / sizeof(kPalette[0]))];
        // Build a closed polygon: center → arc points → center
        dl->PathClear();
        dl->PathLineTo(center);
        int steps = std::max(1, (int)(segPerSlice * frac));
        for (int k = 0; k <= steps; ++k) {
            float a = startAngle + (endAngle - startAngle) * (k / (float)steps);
            dl->PathLineTo(ImVec2(center.x + std::cos(a) * radius,
                                  center.y + std::sin(a) * radius));
        }
        dl->PathFillConvex(col);
        startAngle = endAngle;
    }
}

} // namespace pos::ui::widgets
