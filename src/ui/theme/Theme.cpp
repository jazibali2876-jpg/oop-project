#include "ui/theme/Theme.h"

namespace pos::ui {

void Theme::apply(ThemeMode mode) {
    ImGuiStyle& s = ImGui::GetStyle();
    s.WindowRounding    = 8.0f;
    s.FrameRounding     = 5.0f;
    s.GrabRounding      = 4.0f;
    s.TabRounding       = 5.0f;
    s.PopupRounding     = 6.0f;
    s.ScrollbarRounding = 5.0f;
    s.ChildRounding     = 6.0f;
    s.WindowPadding     = ImVec2(14, 14);
    s.FramePadding      = ImVec2(10, 6);
    s.ItemSpacing       = ImVec2(10, 8);
    s.WindowBorderSize  = 1.0f;
    s.FrameBorderSize   = 0.0f;
    s.ScrollbarSize     = 14.0f;
    s.GrabMinSize       = 12.0f;

    ImVec4* col = s.Colors;
    if (mode == ThemeMode::Dark) {
        col[ImGuiCol_WindowBg]            = ImVec4(0.06f, 0.07f, 0.09f, 1.00f);
        col[ImGuiCol_ChildBg]             = ImVec4(0.09f, 0.10f, 0.13f, 1.00f);
        col[ImGuiCol_PopupBg]             = ImVec4(0.09f, 0.10f, 0.13f, 0.98f);
        col[ImGuiCol_Border]              = ImVec4(0.15f, 0.17f, 0.20f, 1.00f);
        col[ImGuiCol_FrameBg]             = ImVec4(0.10f, 0.12f, 0.16f, 1.00f);
        col[ImGuiCol_FrameBgHovered]      = ImVec4(0.14f, 0.16f, 0.20f, 1.00f);
        col[ImGuiCol_FrameBgActive]       = ImVec4(0.18f, 0.20f, 0.24f, 1.00f);
        col[ImGuiCol_TitleBg]             = ImVec4(0.09f, 0.10f, 0.13f, 1.00f);
        col[ImGuiCol_TitleBgActive]       = ImVec4(0.11f, 0.13f, 0.17f, 1.00f);
        col[ImGuiCol_Text]                = ImVec4(0.95f, 0.96f, 0.97f, 1.00f);
        col[ImGuiCol_TextDisabled]        = ImVec4(0.55f, 0.58f, 0.63f, 1.00f);
        col[ImGuiCol_Button]              = ImVec4(0.16f, 0.18f, 0.22f, 1.00f);
        col[ImGuiCol_ButtonHovered]       = ImVec4(1.00f, 0.42f, 0.21f, 0.85f);
        col[ImGuiCol_ButtonActive]        = ImVec4(1.00f, 0.42f, 0.21f, 1.00f);
        col[ImGuiCol_Header]              = ImVec4(0.16f, 0.18f, 0.22f, 1.00f);
        col[ImGuiCol_HeaderHovered]       = ImVec4(0.20f, 0.23f, 0.28f, 1.00f);
        col[ImGuiCol_HeaderActive]        = ImVec4(0.24f, 0.27f, 0.32f, 1.00f);
        col[ImGuiCol_Separator]           = ImVec4(0.15f, 0.17f, 0.20f, 1.00f);
        col[ImGuiCol_CheckMark]           = ImVec4(1.00f, 0.42f, 0.21f, 1.00f);
        col[ImGuiCol_SliderGrab]          = ImVec4(1.00f, 0.42f, 0.21f, 1.00f);
        col[ImGuiCol_SliderGrabActive]    = ImVec4(1.00f, 0.55f, 0.30f, 1.00f);
        col[ImGuiCol_ScrollbarBg]         = ImVec4(0.06f, 0.07f, 0.09f, 1.00f);
        col[ImGuiCol_ScrollbarGrab]       = ImVec4(0.18f, 0.20f, 0.24f, 1.00f);
        col[ImGuiCol_ScrollbarGrabHovered]= ImVec4(0.24f, 0.27f, 0.32f, 1.00f);
        col[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.28f, 0.31f, 0.36f, 1.00f);
        col[ImGuiCol_Tab]                 = ImVec4(0.11f, 0.13f, 0.17f, 1.00f);
        col[ImGuiCol_TabHovered]          = ImVec4(0.20f, 0.23f, 0.28f, 1.00f);
        col[ImGuiCol_TabSelected]         = ImVec4(0.16f, 0.18f, 0.22f, 1.00f);
        col[ImGuiCol_TableHeaderBg]       = ImVec4(0.11f, 0.13f, 0.17f, 1.00f);
        col[ImGuiCol_TableBorderLight]    = ImVec4(0.15f, 0.17f, 0.20f, 1.00f);
        col[ImGuiCol_TableBorderStrong]   = ImVec4(0.20f, 0.23f, 0.28f, 1.00f);
    } else {
        col[ImGuiCol_WindowBg]            = ImVec4(0.98f, 0.98f, 0.98f, 1.00f);
        col[ImGuiCol_ChildBg]             = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        col[ImGuiCol_PopupBg]             = ImVec4(1.00f, 1.00f, 1.00f, 0.98f);
        col[ImGuiCol_Border]              = ImVec4(0.90f, 0.91f, 0.93f, 1.00f);
        col[ImGuiCol_FrameBg]             = ImVec4(0.96f, 0.97f, 0.98f, 1.00f);
        col[ImGuiCol_FrameBgHovered]      = ImVec4(0.92f, 0.94f, 0.96f, 1.00f);
        col[ImGuiCol_FrameBgActive]       = ImVec4(0.88f, 0.91f, 0.94f, 1.00f);
        col[ImGuiCol_TitleBg]             = ImVec4(0.96f, 0.97f, 0.98f, 1.00f);
        col[ImGuiCol_TitleBgActive]       = ImVec4(0.92f, 0.94f, 0.96f, 1.00f);
        col[ImGuiCol_Text]                = ImVec4(0.07f, 0.09f, 0.15f, 1.00f);
        col[ImGuiCol_TextDisabled]        = ImVec4(0.42f, 0.45f, 0.50f, 1.00f);
        col[ImGuiCol_Button]              = ImVec4(0.94f, 0.95f, 0.96f, 1.00f);
        col[ImGuiCol_ButtonHovered]       = ImVec4(1.00f, 0.42f, 0.21f, 0.20f);
        col[ImGuiCol_ButtonActive]        = ImVec4(1.00f, 0.42f, 0.21f, 0.40f);
        col[ImGuiCol_Header]              = ImVec4(0.94f, 0.95f, 0.96f, 1.00f);
        col[ImGuiCol_HeaderHovered]       = ImVec4(0.88f, 0.91f, 0.94f, 1.00f);
        col[ImGuiCol_HeaderActive]        = ImVec4(0.82f, 0.86f, 0.90f, 1.00f);
        col[ImGuiCol_Separator]           = ImVec4(0.90f, 0.91f, 0.93f, 1.00f);
        col[ImGuiCol_CheckMark]           = ImVec4(1.00f, 0.42f, 0.21f, 1.00f);
        col[ImGuiCol_SliderGrab]          = ImVec4(1.00f, 0.42f, 0.21f, 1.00f);
        col[ImGuiCol_SliderGrabActive]    = ImVec4(1.00f, 0.55f, 0.30f, 1.00f);
        col[ImGuiCol_TableHeaderBg]       = ImVec4(0.94f, 0.95f, 0.96f, 1.00f);
        col[ImGuiCol_TableBorderLight]    = ImVec4(0.90f, 0.91f, 0.93f, 1.00f);
        col[ImGuiCol_TableBorderStrong]   = ImVec4(0.78f, 0.81f, 0.84f, 1.00f);
    }
}

} // namespace pos::ui
