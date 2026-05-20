#include "ui/widgets/Card.h"
#include "ui/theme/Fonts.h"

#include "imgui.h"

namespace pos::ui::widgets {

void Card(const char* title, const char* value, const char* sub, ImU32 accent, ImVec2 size) {
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 8.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
    ImGui::BeginChild(title, size, true);
    {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImVec2 p = ImGui::GetCursorScreenPos();
        dl->AddRectFilled(p, ImVec2(p.x + 4, p.y + 28), accent, 2.0f);

        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 14);
        ImGui::TextDisabled("%s", title);
        ImGui::Spacing();
        if (pos::ui::Fonts::large()) ImGui::PushFont(pos::ui::Fonts::large());
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 6);
        ImGui::Text("%s", value);
        if (pos::ui::Fonts::large()) ImGui::PopFont();
        if (sub && *sub) {
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 6);
            ImGui::TextDisabled("%s", sub);
        }
    }
    ImGui::EndChild();
    ImGui::PopStyleVar(2);
}

} // namespace pos::ui::widgets
