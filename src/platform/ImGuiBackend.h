#pragma once

#include <string>

namespace sf { class RenderWindow; }

namespace pos::platform {

class ImGuiBackend {
public:
    ImGuiBackend();
    ~ImGuiBackend();

    bool init(sf::RenderWindow& window, const std::string& fontDir);
    void newFrame(float deltaSeconds, unsigned fbWidth, unsigned fbHeight);
    void render(sf::RenderWindow& window);
    void shutdown();

private:
    bool initialized_ = false;
};

} // namespace pos::platform
