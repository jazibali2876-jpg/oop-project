// Smart Restaurant POS — entry point.
// All wiring lives in pos::App; this file just constructs and runs it.

#include "App.h"

int main(int /*argc*/, char** /*argv*/) {
    pos::App app;
    return app.run();
}
