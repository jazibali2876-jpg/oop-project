#include "platform/ImGuiBackend.h"

#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"

#include <SFML/Graphics/RenderWindow.hpp>

#include <filesystem>

namespace pos::platform {

ImGuiBackend::ImGuiBackend() = default;

ImGuiBackend::~ImGuiBackend() {
    if (initialized_) shutdown();
}

bool ImGuiBackend::init(sf::RenderWindow& window, const std::string& fontDir) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.IniFilename = nullptr; // do not auto-save UI layout to disk

    const auto regular = std::filesystem::path(fontDir) / "Inter-Regular.ttf";
    const auto bold    = std::filesystem::path(fontDir) / "Inter-Bold.ttf";
    if (std::filesystem::exists(regular)) {
        io.Fonts->AddFontFromFileTTF(regular.string().c_str(), 17.0f);
    } else {
        io.Fonts->AddFontDefault();
    }
    if (std::filesystem::exists(bold)) {
        io.Fonts->AddFontFromFileTTF(bold.string().c_str(), 17.0f);
    }

    auto& style = ImGui::GetStyle();
    style.WindowRounding   = 6.0f;
    style.FrameRounding    = 4.0f;
    style.GrabRounding     = 4.0f;
    style.TabRounding      = 4.0f;
    style.PopupRounding    = 6.0f;
    style.ScrollbarRounding= 4.0f;

    window.setActive(true);
    if (!ImGui_ImplOpenGL3_Init("#version 130")) return false;

    initialized_ = true;
    return true;
}

void ImGuiBackend::newFrame(float deltaSeconds, unsigned fbWidth, unsigned fbHeight) {
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2((float)fbWidth, (float)fbHeight);
    io.DeltaTime   = deltaSeconds > 0.f ? deltaSeconds : 1.f / 60.f;
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();
}

void ImGuiBackend::render(sf::RenderWindow& window) {
    ImGui::Render();
    window.setActive(true);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiBackend::shutdown() {
    if (!initialized_) return;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
    initialized_ = false;
}

} // namespace pos::platform
