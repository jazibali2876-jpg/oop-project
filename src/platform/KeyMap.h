#pragma once

#include <SFML/Window/Keyboard.hpp>
#include "imgui.h"

namespace pos::platform {

ImGuiKey translateKey(sf::Keyboard::Key key);

} // namespace pos::platform
