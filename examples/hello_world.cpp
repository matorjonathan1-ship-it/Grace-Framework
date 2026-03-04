#include "grace/router.hpp"
#include "grace/engine.hpp"
#include <iostream>

// Simplified struct to match the logic
struct WelcomeMessage {
    std::string text;
};

int main() {
    grace::Router router;

    router.get("/", [](auto& req, auto& res) {
        // Only one initializer here to match the struct above
        WelcomeMessage msg{"Welcome to Grace. The Theorem is Proven."};
        res.json(msg);
    });

    // This will work now because we moved engine.hpp into include/grace/
    grace::Engine engine;
    engine.run(router);

    return 0;
}
