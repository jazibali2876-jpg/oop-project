#include "platform/InputBridge.h"
#include "platform/KeyMap.h"

#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>
#include "imgui.h"

namespace pos::platform {

InputBridge::InputBridge() = default;

static int mapMouseButton(sf::Mouse::Button b) {
    switch (b) {
        case sf::Mouse::Button::Left:   return 0;
        case sf::Mouse::Button::Right:  return 1;
        case sf::Mouse::Button::Middle: return 2;
        case sf::Mouse::Button::Extra1: return 3;
        case sf::Mouse::Button::Extra2: return 4;
        default: return -1;
    }
}

void InputBridge::handle(const sf::Event& event) {
    ImGuiIO& io = ImGui::GetIO();

    if (auto* mm = event.getIf<sf::Event::MouseMoved>()) {
        io.AddMousePosEvent((float)mm->position.x, (float)mm->position.y);
        return;
    }
    if (auto* mp = event.getIf<sf::Event::MouseButtonPressed>()) {
        int btn = mapMouseButton(mp->button);
        if (btn >= 0) io.AddMouseButtonEvent(btn, true);
        return;
    }
    if (auto* mr = event.getIf<sf::Event::MouseButtonReleased>()) {
        int btn = mapMouseButton(mr->button);
        if (btn >= 0) io.AddMouseButtonEvent(btn, false);
        return;
    }
    if (auto* mw = event.getIf<sf::Event::MouseWheelScrolled>()) {
        if (mw->wheel == sf::Mouse::Wheel::Vertical) {
            io.AddMouseWheelEvent(0.f, mw->delta);
        } else {
            io.AddMouseWheelEvent(mw->delta, 0.f);
        }
        return;
    }
    if (auto* kp = event.getIf<sf::Event::KeyPressed>()) {
        ImGuiKey k = translateKey(kp->code);
        if (k != ImGuiKey_None) io.AddKeyEvent(k, true);
        io.AddKeyEvent(ImGuiMod_Ctrl,  kp->control);
        io.AddKeyEvent(ImGuiMod_Shift, kp->shift);
        io.AddKeyEvent(ImGuiMod_Alt,   kp->alt);
        io.AddKeyEvent(ImGuiMod_Super, kp->system);
        return;
    }
    if (auto* kr = event.getIf<sf::Event::KeyReleased>()) {
        ImGuiKey k = translateKey(kr->code);
        if (k != ImGuiKey_None) io.AddKeyEvent(k, false);
        io.AddKeyEvent(ImGuiMod_Ctrl,  kr->control);
        io.AddKeyEvent(ImGuiMod_Shift, kr->shift);
        io.AddKeyEvent(ImGuiMod_Alt,   kr->alt);
        io.AddKeyEvent(ImGuiMod_Super, kr->system);
        return;
    }
    if (auto* te = event.getIf<sf::Event::TextEntered>()) {
        if (te->unicode != 0 && te->unicode < 0x10000) {
            io.AddInputCharacter((unsigned)te->unicode);
        }
        return;
    }
    if (event.is<sf::Event::FocusGained>()) {
        io.AddFocusEvent(true);
        return;
    }
    if (event.is<sf::Event::FocusLost>()) {
        io.AddFocusEvent(false);
        return;
    }
}

} // namespace pos::platform
