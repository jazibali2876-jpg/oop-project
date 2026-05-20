#include "ui/widgets/Sidebar.h"
#include "ui/AppContext.h"
#include "ui/Router.h"
#include "ui/state/ToastQueue.h"
#include "ui/theme/Theme.h"

#include "domain/auth/AuthService.h"
#include "domain/auth/Session.h"
#include "domain/common/Capability.h"

#include "imgui.h"

namespace pos::ui::widgets {

static bool NavItem(const char* label, bool active) {
    ImGui::PushStyleColor(ImGuiCol_Button, active
        ? ImVec4(1.0f, 0.42f, 0.21f, 0.8f)
        : ImVec4(0,0,0,0));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, active
        ? ImVec4(1.0f, 0.42f, 0.21f, 0.95f)
        : ImVec4(0.16f, 0.18f, 0.22f, 1.0f));
    bool clicked = ImGui::Button(label, ImVec2(-1, 36));
    ImGui::PopStyleColor(2);
    return clicked;
}

void Sidebar(pos::ui::AppContext& ctx, float width, float height) {
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(width, height));
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration |
                             ImGuiWindowFlags_NoMove |
                             ImGuiWindowFlags_NoSavedSettings |
                             ImGuiWindowFlags_NoBringToFrontOnFocus;
    if (ImGui::Begin("##sidebar", nullptr, flags)) {
        ImGui::Dummy(ImVec2(0, 4));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.42f, 0.21f, 1.0f));
        ImGui::Text("  SMART POS");
        ImGui::PopStyleColor();
        ImGui::TextDisabled("  v1.0");
        ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

        const auto& user = ctx.session.user();
        using pos::domain::Capability;
        using pos::ui::Screen;

        auto cur = ctx.router.current();
        if (NavItem("  Dashboard",  cur == Screen::Dashboard)) ctx.router.go(Screen::Dashboard);
        if (user.can(Capability::ViewMenu)) {
            if (NavItem("  Menu",       cur == Screen::Menu))      ctx.router.go(Screen::Menu);
        }
        if (user.can(Capability::PlaceOrder)) {
            if (NavItem("  New Order",  cur == Screen::Order))     ctx.router.go(Screen::Order);
            if (NavItem("  Billing",    cur == Screen::Billing))   ctx.router.go(Screen::Billing);
        }
        if (user.can(Capability::EditInventory) || user.can(Capability::ViewAnalytics)) {
            if (NavItem("  Inventory",  cur == Screen::Inventory)) ctx.router.go(Screen::Inventory);
        }
        if (user.can(Capability::ManageTables)) {
            if (NavItem("  Tables",     cur == Screen::Tables))    ctx.router.go(Screen::Tables);
        }
        if (user.can(Capability::ViewKitchen)) {
            if (NavItem("  Kitchen",    cur == Screen::Kitchen))   ctx.router.go(Screen::Kitchen);
        }
        if (user.can(Capability::ViewAnalytics)) {
            if (NavItem("  Analytics",  cur == Screen::Analytics)) ctx.router.go(Screen::Analytics);
        }

        // Push logout to the bottom.
        float used = ImGui::GetCursorPosY();
        float remaining = ImGui::GetContentRegionAvail().y - 40.f;
        if (remaining > 0) ImGui::Dummy(ImVec2(0, remaining));
        ImGui::Separator();
        if (NavItem("  Logout", false)) {
            ctx.auth.logout(ctx.session);
            ctx.router.go(Screen::Login);
            ctx.toasts.info("Signed out");
        }
        (void)used;
    }
    ImGui::End();
}

} // namespace pos::ui::widgets
