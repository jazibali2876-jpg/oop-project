#include "ui/screens/Screens.h"
#include "ui/AppContext.h"
#include "ui/Router.h"
#include "ui/widgets/Card.h"
#include "ui/widgets/Chart.h"
#include "ui/theme/Theme.h"

#include "domain/analytics/AnalyticsService.h"
#include "domain/inventory/InventoryService.h"
#include "domain/menu/MenuService.h"
#include "domain/order/OrderService.h"

#include "imgui.h"

#include <cstdio>

namespace pos::ui::screens {

void DrawDashboard(pos::ui::AppContext& ctx) {
    auto today = ctx.analytics.today();
    auto week  = ctx.analytics.thisWeek();
    auto active = ctx.orders.active();
    auto low   = ctx.inventory.lowStock();

    char revBuf[32], ordBuf[16], lowBuf[16];
    std::snprintf(revBuf, sizeof(revBuf), "%s", today.revenue().format().c_str());
    std::snprintf(ordBuf, sizeof(ordBuf), "%d", (int)active.size());
    std::snprintf(lowBuf, sizeof(lowBuf), "%d", (int)low.size());

    // Top row: metric cards
    float cardW = (ImGui::GetContentRegionAvail().x - 32) / 4.0f;
    cardW = std::max(cardW, 160.f);
    pos::ui::widgets::Card("Today's Revenue", revBuf, "vs yesterday",
                           pos::ui::Theme::brandAccent, ImVec2(cardW, 110));
    ImGui::SameLine();
    pos::ui::widgets::Card("Active Orders", ordBuf, "pending in kitchen",
                           pos::ui::Theme::colorInfo, ImVec2(cardW, 110));
    ImGui::SameLine();
    pos::ui::widgets::Card("Low Stock", lowBuf, "ingredients below threshold",
                           low.empty() ? pos::ui::Theme::colorSuccess : pos::ui::Theme::colorWarning,
                           ImVec2(cardW, 110));
    ImGui::SameLine();
    auto top = today.topItems(1);
    std::string topName = "—";
    if (!top.empty()) {
        try {
            topName = ctx.menu.byId(pos::domain::MenuItemId{top.front().first}).name();
        } catch (...) {}
    }
    char topVal[32];
    std::snprintf(topVal, sizeof(topVal), "%d sold", top.empty() ? 0 : top.front().second);
    pos::ui::widgets::Card("Top Item Today", topName.c_str(), topVal,
                           pos::ui::Theme::colorSuccess, ImVec2(cardW, 110));

    ImGui::Spacing(); ImGui::Spacing();

    // Two columns: top items list (left), revenue chart (right)
    float halfW = (ImGui::GetContentRegionAvail().x - 14) * 0.5f;
    ImGui::BeginChild("##topList", ImVec2(halfW, 240), true);
    {
        ImGui::TextDisabled("Top selling today");
        ImGui::Separator();
        auto items = today.topItems(8);
        if (items.empty()) {
            ImGui::TextDisabled("No sales yet today.");
        } else {
            int rank = 1;
            for (auto& [id, qty] : items) {
                std::string nm;
                try { nm = ctx.menu.byId(pos::domain::MenuItemId{id}).name(); }
                catch (...) { nm = "Item #" + std::to_string(id); }
                ImGui::Text("%d. %-26.26s  x%d", rank++, nm.c_str(), qty);
            }
        }
    }
    ImGui::EndChild();
    ImGui::SameLine();
    ImGui::BeginChild("##revChart", ImVec2(halfW, 240), true);
    {
        ImGui::TextDisabled("Revenue, last 7 days (drawn manually with ImDrawList)");
        ImGui::Separator();
        std::vector<float> series(7, 0.f);
        auto daily = ctx.analytics.daily(pos::domain::DateTime::now().startOfDay().addSeconds(-6 * 86400),
                                         pos::domain::DateTime::now());
        for (auto& dp : daily) {
            int dayIdx = (int)((dp.day.epoch() - pos::domain::DateTime::now().startOfDay().addSeconds(-6 * 86400).epoch()) / 86400);
            if (dayIdx >= 0 && dayIdx < 7) series[dayIdx] = (float)dp.revenue.toDouble();
        }
        pos::ui::widgets::LineChart(series, ImVec2(halfW - 30, 170));
    }
    ImGui::EndChild();

    ImGui::Spacing();

    // Low stock alerts panel
    ImGui::BeginChild("##lowStock", ImVec2(-1, 200), true);
    {
        ImGui::TextDisabled("Low stock alerts");
        ImGui::Separator();
        if (low.empty()) {
            ImGui::TextDisabled("All ingredients above threshold.");
        } else {
            if (ImGui::BeginTable("low", 4, ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_RowBg)) {
                ImGui::TableSetupColumn("Ingredient");
                ImGui::TableSetupColumn("Stock");
                ImGui::TableSetupColumn("Threshold");
                ImGui::TableSetupColumn("Status");
                ImGui::TableHeadersRow();
                for (auto& ing : low) {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0); ImGui::Text("%s", ing.name().c_str());
                    ImGui::TableSetColumnIndex(1); ImGui::Text("%.1f %s", ing.stock(), ing.unit().c_str());
                    ImGui::TableSetColumnIndex(2); ImGui::Text("%.1f", ing.reorderThreshold());
                    ImGui::TableSetColumnIndex(3);
                    ImGui::PushStyleColor(ImGuiCol_Text, ImGui::ColorConvertU32ToFloat4(pos::ui::Theme::colorWarning));
                    ImGui::Text("LOW");
                    ImGui::PopStyleColor();
                }
                ImGui::EndTable();
            }
        }
    }
    ImGui::EndChild();
    (void)week;
}

} // namespace pos::ui::screens
