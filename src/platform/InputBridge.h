#pragma once

namespace sf { class Event; }

namespace pos::platform {

class InputBridge {
public:
    InputBridge();

    // Convert a single SFML 3 sf::Event into the corresponding ImGuiIO events.
    void handle(const sf::Event& event);
};

} // namespace pos::platform
