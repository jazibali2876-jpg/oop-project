#include "ui/screens/Screens.h"
#include "ui/AppContext.h"
#include "ui/widgets/Card.h"
#include "ui/widgets/Chart.h"
#include "ui/theme/Theme.h"

#include "domain/analytics/AnalyticsService.h"
#include "domain/menu/MenuService.h"

#include "imgui.h"

#include <cstdio>

namespace pos::ui::screens {

void DrawAnalytics(pos::ui::AppContext& ctx) {
    using namespace pos::domain;

    static int rangeChoice = 1;  // 0=today, 1=week, 2=month
    const char* ranges[] = {"Today", "Last 7 days", "Last 30 days"};

    ImGui::Text("Analytics");
    ImGui::SameLine(ImGui::GetContentRegionAvail().x - 220);
    ImGui::PushItemWidth(220);
    ImGui::Combo("##range", &rangeChoice, ranges, IM_ARRAYSIZE(ranges));
    ImGui::PopItemWidth();
    ImGui::Separator();

    Report rep = (rangeChoice == 0) ? ctx.analytics.today()
               : (rangeChoice == 1) ? ctx.analytics.thisWeek()
                                    : ctx.analytics.thisMonth();

    // Top metrics
    char revBuf[32], ordBuf[16], aovBuf[32];
    std::snprintf(revBuf, sizeof(revBuf), "%s", rep.revenue().format().c_str());
    std::snprintf(ordBuf, sizeof(ordBuf), "%d", rep.orderCount());
    double aov = rep.orderCount() > 0
        ? rep.revenue().toDouble() / (double)rep.orderCount() : 0.0;
    std::snprintf(aovBuf, sizeof(aovBuf), "$%.2f", aov);

    float cardW = (ImGui::GetContentRegionAvail().x - 30) / 3.f;
    pos::ui::widgets::Card("Revenue",    revBuf, ranges[rangeChoice], pos::ui::Theme::brandAccent, ImVec2(cardW, 100));
    ImGui::SameLine();
    pos::ui::widgets::Card("Orders",     ordBuf, "total in range",    pos::ui::Theme::colorInfo,   ImVec2(cardW, 100));
    ImGui::SameLine();
    pos::ui::widgets::Card("Avg Order",  aovBuf, "revenue / orders",  pos::ui::Theme::colorSuccess,ImVec2(cardW, 100));

    ImGui::Spacing();

    // Revenue per day bar chart
    auto daily = ctx.analytics.daily(rep.from(), rep.to());
    std::vector<pos::ui::widgets::BarDatum> bars;
    for (const auto& dp : daily) {
        char lbl[12];
        std::snprintf(lbl, sizeof(lbl), "%s", dp.day.format("%m/%d").c_str());
        bars.push_back({lbl, (float)dp.revenue.toDouble(), 0});
    }

    float halfW = (ImGui::GetContentRegionAvail().x - 14) * 0.5f;
    ImGui::BeginChild("##revbar", ImVec2(halfW, 280), true);
    ImGui::TextDisabled("Revenue by day  (ImDrawList bar chart)");
    ImGui::Separator();
    if (bars.empty()) ImGui::TextDisabled("No data in range yet.");
    else pos::ui::widgets::BarChart(bars, ImVec2(halfW - 30, 220));
    ImGui::EndChild();

    ImGui::SameLine();
    ImGui::BeginChild("##pie", ImVec2(halfW, 280), true);
    ImGui::TextDisabled("Top items distribution  (ImDrawList pie chart)");
    ImGui::Separator();
    auto items = rep.topItems(5);
    std::vector<pos::ui::widgets::PieSlice> slices;
    for (auto& [id, qty] : items) {
        std::string nm;
        try { nm = ctx.menu.byId(MenuItemId{id}).name(); }
        catch (...) { nm = "Item #" + std::to_string(id); }
        slices.push_back({nm, (float)qty, 0});
    }
    if (slices.empty()) ImGui::TextDisabled("No item data yet.");
    else pos::ui::widgets::PieChart(slices, 90.f);

    ImGui::SetCursorPos(ImVec2(220, 30));
    ImGui::BeginGroup();
    for (std::size_t i = 0; i < items.size(); ++i) {
        ImGui::Text(" %s  (%d)", slices[i].label.c_str(), (int)slices[i].value);
    }
    ImGui::EndGroup();
    ImGui::EndChild();

    ImGui::Spacing();

    // Peak-hours line chart
    ImGui::BeginChild("##peak", ImVec2(-1, 200), true);
    ImGui::TextDisabled("Orders by hour of day  (ImDrawList line chart)");
    ImGui::Separator();
    std::vector<float> hours;
    hours.reserve(24);
    for (int h = 0; h < 24; ++h) hours.push_back((float)rep.hourBuckets()[h]);
    pos::ui::widgets::LineChart(hours, ImVec2(-1, 140));
    ImGui::EndChild();
}

} // namespace pos::ui::screens
