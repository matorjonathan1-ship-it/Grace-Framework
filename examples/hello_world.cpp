#include "../include/grace/router.hpp"
#include <iostream>

int main() {
    grace::Router router;

    // This sets up the 'welcome' radio frequency
    router.get("/api/v1/welcome", [](auto& req, auto& res) {
        res.json("{\"text\":\"Welcome to Grace. The Theorem is Proven.\", \"code\":1}");
    });

    std::cout << "⚓ Grace Engine deployed on port 8080..." << std::endl;

    grace::Engine engine;
    engine.run(router); // Passing the router to the engine room

    return 0;
}
