#pragma once

#include <memory>

namespace pos {

class App {
public:
    App();
    ~App();

    int run();

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace pos
