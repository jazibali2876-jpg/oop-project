#include "ui/widgets/TopBar.h"
#include "ui/AppContext.h"
#include "ui/Router.h"
#include "ui/theme/Theme.h"

#include "domain/auth/Session.h"
#include "domain/auth/User.h"
#include "domain/common/DateTime.h"
#include "domain/persistence/SettingsRepository.h"

#include "imgui.h"

namespace pos::ui::widgets {

void TopBar(pos::ui::AppContext& ctx, float left, float width, float height) {
    ImGui::SetNextWindowPos(ImVec2(left, 0));
    ImGui::SetNextWindowSize(ImVec2(width, height));
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration |
                             ImGuiWindowFlags_NoMove |
                             ImGuiWindowFlags_NoSavedSettings |
                             ImGuiWindowFlags_NoBringToFrontOnFocus;
    if (ImGui::Begin("##topbar", nullptr, flags)) {
        // Screen title
        const char* screenName = "";
        switch (ctx.router.current()) {
            case Screen::Dashboard:  screenName = "Dashboard"; break;
            case Screen::Menu:       screenName = "Menu Management"; break;
            case Screen::Order:      screenName = "New Order"; break;
            case Screen::Billing:    screenName = "Billing & Receipts"; break;
            case Screen::Inventory:  screenName = "Inventory"; break;
            case Screen::Tables:     screenName = "Tables"; break;
            case Screen::Kitchen:    screenName = "Kitchen Display"; break;
            case Screen::Analytics:  screenName = "Analytics"; break;
            default: break;
        }
        ImGui::SetCursorPos(ImVec2(20, 12));
        ImGui::Text("%s", screenName);

        // Right-aligned: clock, theme toggle, user
        const char* themeBtn = ctx.darkTheme ? " Light " : " Dark ";
        float rightPad = 20.f;
        float btnW = 70.f;
        std::string clockStr = pos::domain::DateTime::now().format("%H:%M:%S");
        float clockW = ImGui::CalcTextSize(clockStr.c_str()).x + 10.f;

        const std::string userLabel = "User: " + ctx.session.user().displayName()
                                    + " (" + ctx.session.user().roleName() + ")";
        float userW  = ImGui::CalcTextSize(userLabel.c_str()).x + 10.f;

        float xRight = width - rightPad;
        ImGui::SetCursorPos(ImVec2(xRight - userW, 14));
        ImGui::TextDisabled("%s", userLabel.c_str());

        ImGui::SetCursorPos(ImVec2(xRight - userW - btnW - 8, 8));
        if (ImGui::Button(themeBtn, ImVec2(btnW, 28))) {
            ctx.darkTheme = !ctx.darkTheme;
            Theme::apply(ctx.darkTheme ? ThemeMode::Dark : ThemeMode::Light);
            ctx.settings.set("theme", ctx.darkTheme ? "Dark" : "Light");
        }
        ImGui::SetCursorPos(ImVec2(xRight - userW - btnW - clockW - 16, 14));
        ImGui::Text("%s", clockStr.c_str());
    }
    ImGui::End();
}

} // namespace pos::ui::widgets
