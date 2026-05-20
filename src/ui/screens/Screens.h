#pragma once

namespace pos::ui {
    class AppContext;
}

namespace pos::ui::screens {

void DrawSplash    (pos::ui::AppContext& ctx);
void DrawLogin     (pos::ui::AppContext& ctx);
void DrawDashboard (pos::ui::AppContext& ctx);
void DrawMenu      (pos::ui::AppContext& ctx);
void DrawOrder     (pos::ui::AppContext& ctx);
void DrawBilling   (pos::ui::AppContext& ctx);
void DrawInventory (pos::ui::AppContext& ctx);
void DrawTables    (pos::ui::AppContext& ctx);
void DrawKitchen   (pos::ui::AppContext& ctx);
void DrawAnalytics (pos::ui::AppContext& ctx);

} // namespace pos::ui::screens
