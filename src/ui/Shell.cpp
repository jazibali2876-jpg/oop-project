#include "ui/Shell.h"
#include "ui/AppContext.h"
#include "ui/Router.h"
#include "ui/screens/Screens.h"
#include "ui/widgets/Sidebar.h"
#include "ui/widgets/TopBar.h"

#include "domain/auth/Session.h"

#include "imgui.h"

namespace pos::ui {

void Shell::draw(AppContext& ctx) {
    // Pre-auth screens are full-window, no chrome.
    if (ctx.router.current() == Screen::Splash) {
        pos::ui::screens::DrawSplash(ctx);
        return;
    }
    if (ctx.router.current() == Screen::Login || !ctx.session.isAuthenticated()) {
        pos::ui::screens::DrawLogin(ctx);
        return;
    }

    const float sidebarW = 220.f;
    const float topbarH  = 56.f;

    pos::ui::widgets::Sidebar(ctx, sidebarW, ctx.screenH);
    pos::ui::widgets::TopBar (ctx, sidebarW, ctx.screenW - sidebarW, topbarH);

    // Main content area
    ImGui::SetNextWindowPos(ImVec2(sidebarW, topbarH));
    ImGui::SetNextWindowSize(ImVec2(ctx.screenW - sidebarW, ctx.screenH - topbarH));
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration |
                             ImGuiWindowFlags_NoMove |
                             ImGuiWindowFlags_NoSavedSettings |
                             ImGuiWindowFlags_NoBringToFrontOnFocus;
    if (ImGui::Begin("##content", nullptr, flags)) {
        switch (ctx.router.current()) {
            case Screen::Dashboard:  pos::ui::screens::DrawDashboard(ctx); break;
            case Screen::Menu:       pos::ui::screens::DrawMenu(ctx);      break;
            case Screen::Order:      pos::ui::screens::DrawOrder(ctx);     break;
            case Screen::Billing:    pos::ui::screens::DrawBilling(ctx);   break;
            case Screen::Inventory:  pos::ui::screens::DrawInventory(ctx); break;
            case Screen::Tables:     pos::ui::screens::DrawTables(ctx);    break;
            case Screen::Kitchen:    pos::ui::screens::DrawKitchen(ctx);   break;
            case Screen::Analytics:  pos::ui::screens::DrawAnalytics(ctx); break;
            default: break;
        }
    }
    ImGui::End();
}

} // namespace pos::ui
