#include "../include/grace/core.hpp"
#include "../include/grace/reflect.hpp"
#include <string>

// 1. Define a Data Model using the "Strict" Reflection Engine
struct Message {
    std::string text;
    int code;
    
    // The Commodore's Choice: Explicit reflection for safety
    GRACE_REFLECT(text, code);
};

int main() {
    // 2. Initialize the Grace Application
    auto app = grace::create_app();

    // 3. Define a Route - The "Theorem" being proved here:
    // The lambda function MUST match the GraceHandler concept defined in router.hpp
    app->get("/hello", [](auto& req, auto& res) {
        Message msg{.text = "Hello from Grace!", .code = 200};
        
        // This will eventually use the reflection engine to auto-serialize to JSON
        res.json(msg);
    });

    // 4. Start the Engine
    return app->listen(8080);
}
