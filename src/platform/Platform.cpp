#include "platform/Platform.h"
#include "platform/InputBridge.h"

#include <SFML/Window/Event.hpp>
#include <SFML/Window/ContextSettings.hpp>

namespace pos::platform {

Platform::Platform() = default;
Platform::~Platform() = default;

bool Platform::init(const Config& cfg) {
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antiAliasingLevel = 0;
    settings.majorVersion = 3;
    settings.minorVersion = 3;

    window_ = std::make_unique<sf::RenderWindow>(
        sf::VideoMode({cfg.width, cfg.height}),
        cfg.title,
        sf::Style::Default,
        sf::State::Windowed,
        settings
    );
    if (!window_->isOpen()) return false;

    window_->setVerticalSyncEnabled(true);
    window_->setActive(true);
    return true;
}

bool Platform::pumpEvents(InputBridge& bridge) {
    bool keepRunning = true;
    while (auto evtOpt = window_->pollEvent()) {
        const sf::Event& evt = *evtOpt;
        bridge.handle(evt);
        if (evt.is<sf::Event::Closed>()) {
            keepRunning = false;
        }
    }
    return keepRunning;
}

void Platform::clear() {
    window_->setActive(true);
    window_->clear(sf::Color(15, 17, 21));
}

void Platform::display() {
    window_->display();
}

sf::RenderWindow& Platform::window() { return *window_; }
const sf::RenderWindow& Platform::window() const { return *window_; }

float Platform::deltaSeconds() {
    float dt = clock_.getElapsedTime().asSeconds();
    clock_.restart();
    if (dt <= 0.f) dt = 1.f / 60.f;
    if (dt > 0.25f) dt = 0.25f;
    return dt;
}

sf::Vector2u Platform::framebufferSize() const {
    return window_->getSize();
}

} // namespace pos::platform
