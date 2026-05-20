#include "ui/screens/Screens.h"
#include "ui/AppContext.h"
#include "ui/Router.h"
#include "ui/state/ToastQueue.h"
#include "ui/theme/Fonts.h"

#include "domain/auth/AuthService.h"
#include "domain/auth/Session.h"
#include "domain/common/Exceptions.h"
#include "domain/persistence/SettingsRepository.h"

#include "imgui.h"

#include <cstring>

namespace pos::ui::screens {

void DrawLogin(pos::ui::AppContext& ctx) {
    static char  usernameBuf[64] = {};
    static char  passwordBuf[64] = {};
    static bool  rememberMe      = false;
    static bool  initialized     = false;
    static bool  shakeOnError    = false;
    static float shakeTimer      = 0.f;

    if (!initialized) {
        auto remembered = ctx.settings.get("rememberedUser");
        if (remembered) {
            std::strncpy(usernameBuf, remembered->c_str(), sizeof(usernameBuf) - 1);
            rememberMe = true;
        }
        initialized = true;
    }

    if (shakeTimer > 0.f) shakeTimer -= ctx.deltaSeconds;
    float shakeX = (shakeTimer > 0.f) ? (((int)(shakeTimer * 40)) % 2 ? 6.f : -6.f) : 0.f;

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(ctx.screenW, ctx.screenH));
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration |
                             ImGuiWindowFlags_NoMove |
                             ImGuiWindowFlags_NoSavedSettings;
    if (ImGui::Begin("##login", nullptr, flags)) {
        float boxW = 420.f, boxH = 420.f;
        float cx = ctx.screenW * 0.5f + shakeX;
        float cy = ctx.screenH * 0.5f;
        ImGui::SetCursorPos(ImVec2(cx - boxW * 0.5f, cy - boxH * 0.5f));
        ImGui::BeginChild("loginbox", ImVec2(boxW, boxH), true);
        {
            ImGui::Dummy(ImVec2(0, 14));
            if (pos::ui::Fonts::large()) ImGui::PushFont(pos::ui::Fonts::large());
            const char* title = "Smart POS";
            ImVec2 sz = ImGui::CalcTextSize(title);
            ImGui::SetCursorPosX((boxW - sz.x) * 0.5f);
            ImGui::Text("%s", title);
            if (pos::ui::Fonts::large()) ImGui::PopFont();
            ImGui::SetCursorPosX((boxW - ImGui::CalcTextSize("Welcome back").x) * 0.5f);
            ImGui::TextDisabled("Welcome back");
            ImGui::Spacing(); ImGui::Spacing();

            ImGui::TextDisabled("Username");
            ImGui::PushItemWidth(-1);
            ImGui::InputText("##user", usernameBuf, sizeof(usernameBuf));
            ImGui::PopItemWidth();

            ImGui::Spacing();
            ImGui::TextDisabled("Password");
            ImGui::PushItemWidth(-1);
            ImGui::InputText("##pwd", passwordBuf, sizeof(passwordBuf),
                             ImGuiInputTextFlags_Password);
            ImGui::PopItemWidth();

            ImGui::Spacing();
            ImGui::Checkbox("Remember me", &rememberMe);
            ImGui::Spacing(); ImGui::Spacing();

            bool submit = ImGui::Button("Sign in", ImVec2(-1, 40));
            if (ImGui::IsKeyPressed(ImGuiKey_Enter, false)) submit = true;
            if (submit) {
                try {
                    auto u = ctx.auth.login(usernameBuf, passwordBuf);
                    if (rememberMe) ctx.settings.set("rememberedUser", usernameBuf);
                    ctx.toasts.success("Welcome", "Signed in as " + u->displayName());
                    std::memset(passwordBuf, 0, sizeof(passwordBuf));
                    ctx.session.set(std::move(u));
                    ctx.router.go(pos::ui::Screen::Dashboard);
                } catch (const pos::domain::DomainException& e) {
                    ctx.toasts.danger("Sign-in failed", e.what());
                    shakeTimer = 0.4f;
                    std::memset(passwordBuf, 0, sizeof(passwordBuf));
                }
            }

            ImGui::Spacing(); ImGui::Spacing();
            ImGui::TextDisabled("Tip: default admin is admin / admin123");
        }
        ImGui::EndChild();
    }
    ImGui::End();
}

} // namespace pos::ui::screens
