#include "../include/grace/core.hpp"
#include "../include/grace/json.hpp" // Now we include our new JSON engine
#include <string>

// 1. Define the Data Model
struct WelcomeMessage {
    std::string message;
    int version;
    
    // Explicitly choice: only reflect what's safe for the web
    GRACE_REFLECT(message, version);
};

int main() {
    auto app = grace::create_app();

    // 2. The Route Theorem in action
    app->get("/api/v1/welcome", [](auto& req, auto& res) {
        WelcomeMessage msg{
            .message = "Welcome to Grace. The Theorem is Proven.",
            .version = 1
        };
        
        // 3. Automated Serialization using the 'Strict' engine
        // This call is proven at compile-time by our 'Reflected' concept
        res.json(grace::serialize(msg));
    });

    return app->listen(8080);
}
