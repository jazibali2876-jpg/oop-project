#include "ui/screens/Screens.h"
#include "ui/AppContext.h"
#include "ui/Router.h"
#include "ui/anim/Animation.h"
#include "ui/theme/Fonts.h"
#include "ui/theme/Theme.h"

#include "imgui.h"

namespace pos::ui::screens {

void DrawSplash(pos::ui::AppContext& ctx) {
    static float elapsed = 0.f;
    elapsed += ctx.deltaSeconds;

    float duration = 1.6f;
    float t = pos::ui::Animation::clamp01(elapsed / duration);

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(ctx.screenW, ctx.screenH));
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration |
                             ImGuiWindowFlags_NoMove |
                             ImGuiWindowFlags_NoSavedSettings;
    if (ImGui::Begin("##splash", nullptr, flags)) {
        float cx = ctx.screenW * 0.5f;
        float cy = ctx.screenH * 0.5f;
        ImDrawList* dl = ImGui::GetWindowDrawList();

        // Pulsing accent rectangle (logo placeholder)
        float pulse = 0.6f + 0.4f * pos::ui::Animation::easeOutQuad(t);
        ImU32 brand = pos::ui::Theme::brandAccent;
        ImU32 brandDim = IM_COL32(255, 107, 53, (int)(255 * pulse));
        dl->AddRectFilled(ImVec2(cx - 60, cy - 80), ImVec2(cx + 60, cy + 40),
                          brandDim, 14.f);

        ImGui::PushFont(pos::ui::Fonts::large() ? pos::ui::Fonts::large() : ImGui::GetFont());
        const char* title = "Smart Restaurant POS";
        ImVec2 sz = ImGui::CalcTextSize(title);
        ImGui::SetCursorPos(ImVec2(cx - sz.x * 0.5f, cy + 70));
        ImGui::Text("%s", title);
        ImGui::PopFont();

        const char* sub = "Loading menu, inventory, customers...";
        ImVec2 ssz = ImGui::CalcTextSize(sub);
        ImGui::SetCursorPos(ImVec2(cx - ssz.x * 0.5f, cy + 110));
        ImGui::TextDisabled("%s", sub);

        ImGui::SetCursorPos(ImVec2(cx - 150, cy + 140));
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImGui::ColorConvertU32ToFloat4(brand));
        ImGui::ProgressBar(t, ImVec2(300, 8), "");
        ImGui::PopStyleColor();

        (void)brand;
    }
    ImGui::End();

    if (t >= 1.f) {
        ctx.router.go(pos::ui::Screen::Login);
        elapsed = 0.f; // reset for next time it appears
    }
}

} // namespace pos::ui::screens
