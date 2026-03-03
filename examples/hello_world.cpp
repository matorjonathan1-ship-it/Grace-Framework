#include "../include/grace/router.hpp"
#include <iostream>

// 1. Define the data structure the Engine can understand
struct WelcomeMessage {
    std::string message;
    std::string to_json() const {
        return "{\"message\":\"" + message + "\"}";
    }
};
int main() {
    grace::Router router;

    router.get("/api/v1/welcome", [](auto& req, auto& res) {
        // 2. Use the struct instead of a raw string
        WelcomeMessage msg{"Welcome to Grace. The Theorem is Proven.", 1};
        res.json(msg);
    });

    std::cout << "⚓ Grace Engine deployed on port 8080..." << std::endl;

    // 3. Proper Engine initialization
    grace::Engine engine;
    engine.run(router);

    return 0;
}
