#include "ui/state/ToastQueue.h"
#include "ui/anim/Animation.h"
#include "ui/theme/Theme.h"

#include "imgui.h"

#include <algorithm>

namespace pos::ui {

void ToastQueue::push(Toast t) {
    queue_.push_back(std::move(t));
    if (queue_.size() > 5) queue_.pop_front();
}

void ToastQueue::info   (std::string title, std::string body) { push({std::move(title), std::move(body), ToastKind::Info,    4.0f, 0.f}); }
void ToastQueue::success(std::string title, std::string body) { push({std::move(title), std::move(body), ToastKind::Success, 4.0f, 0.f}); }
void ToastQueue::warn   (std::string title, std::string body) { push({std::move(title), std::move(body), ToastKind::Warning, 5.0f, 0.f}); }
void ToastQueue::danger (std::string title, std::string body) { push({std::move(title), std::move(body), ToastKind::Danger,  6.0f, 0.f}); }

static ImU32 toastColor(ToastKind k) {
    switch (k) {
        case ToastKind::Success: return Theme::colorSuccess;
        case ToastKind::Warning: return Theme::colorWarning;
        case ToastKind::Danger:  return Theme::colorDanger;
        default:                 return Theme::colorInfo;
    }
}

static const char* toastIcon(ToastKind k) {
    switch (k) {
        case ToastKind::Success: return "[OK]";
        case ToastKind::Warning: return "[!]";
        case ToastKind::Danger:  return "[X]";
        default:                 return "[i]";
    }
}

void ToastQueue::tickAndDraw(float dt, float screenW) {
    for (auto& t : queue_) {
        t.age += dt;
        t.ttl -= dt;
    }
    queue_.erase(std::remove_if(queue_.begin(), queue_.end(),
        [](const Toast& t){ return t.ttl <= 0.f; }), queue_.end());

    const float W = 320.f;
    const float pad = 14.f;
    float y = pad;
    int idx = 0;
    for (const auto& t : queue_) {
        float slideIn = Animation::easeOutQuad(Animation::clamp01(t.age * 4.f));
        float xOffset = (1.f - slideIn) * (W + 40.f);
        ImGui::SetNextWindowPos(ImVec2(screenW - W - pad + xOffset, y), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(W, 0));
        ImGui::PushID(idx++);
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration |
                                 ImGuiWindowFlags_NoMove |
                                 ImGuiWindowFlags_NoSavedSettings |
                                 ImGuiWindowFlags_NoFocusOnAppearing |
                                 ImGuiWindowFlags_AlwaysAutoResize;
        ImGui::PushStyleColor(ImGuiCol_Border, ImGui::ColorConvertU32ToFloat4(toastColor(t.kind)));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 2.0f);
        if (ImGui::Begin("##toast", nullptr, flags)) {
            ImGui::Text("%s %s", toastIcon(t.kind), t.title.c_str());
            if (!t.body.empty()) {
                ImGui::TextWrapped("%s", t.body.c_str());
            }
        }
        ImGui::End();
        ImGui::PopStyleVar();
        ImGui::PopStyleColor();
        ImGui::PopID();
        y += 80.f;
    }
}

} // namespace pos::ui
