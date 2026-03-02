#include "include/grace.hpp"
#include <cstring>

int main() {
    grace::App app;

    // A real-world health check route
    app.get("/api/v1/health", [](auto& req, int fd, auto latency) {
        char response_body[128];
        snprintf(response_body, sizeof(response_body), 
            "{\"status\":\"ok\",\"engine\":\"io_uring\",\"latency_us\":%lld}", latency);
        
        char buf[512];
        int len = snprintf(buf, sizeof(buf),
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json\r\n"
            "X-Response-Time: %lldus\r\n"
            "Content-Length: %zu\r\n"
            "Connection: close\r\n\r\n%s", 
            latency, strlen(response_body), response_body);
            
        send(fd, buf, len, 0);
    });

    // The port and the startup
    app.listen(8080);

    return 0;
}
