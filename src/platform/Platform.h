#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>

#include <memory>
#include <string>

namespace pos::platform {

class InputBridge;

struct Config {
    unsigned width  = 1280;
    unsigned height = 800;
    std::string title = "Smart Restaurant POS";
};

class Platform {
public:
    Platform();
    ~Platform();

    bool init(const Config& cfg);

    // Drains the SFML event queue, forwards events to ImGui via the bridge.
    // Returns false if a Closed event arrived (caller should exit the loop).
    bool pumpEvents(InputBridge& bridge);

    void clear();
    void display();

    sf::RenderWindow& window();
    const sf::RenderWindow& window() const;

    float deltaSeconds();          // resets internal clock
    sf::Vector2u framebufferSize() const;

private:
    std::unique_ptr<sf::RenderWindow> window_;
    sf::Clock clock_;
};

} // namespace pos::platform
