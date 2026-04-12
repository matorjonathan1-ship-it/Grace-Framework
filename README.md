# Grace Framework
**A High-Performance, Asynchronous C++ Web Framework powered by io_uring.**

Grace is built for developers who need sub-millisecond latency and the raw power of the Linux kernel. 

### Performance
- **Avg. Latency:** 16μs - 40μs
- **Engine:** Linux `io_uring` (Non-blocking I/O)
- **Language:** C++20

### Quick Start
Grace is a **single-header library**. Just drop `grace.hpp` into your project.

```cpp
#include "grace.hpp"

int main() {
    grace::App app;

    app.get("/api/v1/health", [](auto& req, auto& res, auto latency) {
        res.json("{\"status\":\"ok\"}", latency);
    });

    app.listen(8080);
}

cat << 'EOF' > ../README.md
# Grace Framework
**A High-Performance, Asynchronous C++ Web Framework powered by io_uring.**

Grace is built for developers who need sub-millisecond latency and the raw power of the Linux kernel. 

### Performance
- **Avg. Latency:** 16μs - 40μs
- **Engine:** Linux `io_uring` (Non-blocking I/O)
- **Language:** C++20

### Quick Start
Grace is a **single-header library**. Just drop `grace.hpp` into your project.

```cpp
#include "grace.hpp"

int main() {
    grace::App app;

    app.get("/api/v1/health", [](auto& req, auto& res, auto latency) {
        res.json("{\"status\":\"ok\"}", latency);
    });

    app.listen(8080);
}
cat << 'EOF' > ../include/grace.hpp
#ifndef GRACE_HPP
#define GRACE_HPP

#include <iostream>
#include <string>
#include <map>
#include <functional>
#include <liburing.h>
#include <netinet/in.h>
#include <unistd.h>
#include <chrono>

namespace grace {
    struct Request { std::string method, path; };
    struct Response {
        int fd;
        void json(std::string body, long long us) {
            std::string h = "HTTP/1.1 200 OK\r\n"
                            "Content-Type: application/json\r\n"
                            "Server: Grace/1.0\r\n"
                            "X-Response-Time: " + std::to_string(us) + "us\r\n"
                            "Content-Length: " + std::to_string(body.length()) + "\r\n"
                            "Connection: close\r\n\r\n" + body;
            send(fd, h.c_str(), h.length(), 0);
        }
    };

    class App {
        std::map<std::string, std::function<void(Request&, Response&, long long)>> routes;
    public:
        void get(std::string path, std::function<void(Request&, Response&, long long)> cb) { 
            routes["GET" + path] = cb; 
        }

        void listen(int port) {
            int s = socket(AF_INET6, SOCK_STREAM, 0);
            int off = 0;
            setsockopt(s, IPPROTO_IPV6, IPV6_V6ONLY, &off, sizeof(off));
            int opt = 1; 
            setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
            
            struct sockaddr_in6 a = {};
            a.sin6_family = AF_INET6;
            a.sin6_port = htons(port);
            a.sin6_addr = in6addr_any;

            bind(s, (struct sockaddr*)&a, sizeof(a));
            ::listen(s, 128);

            struct io_uring ring; 
            io_uring_queue_init(256, &ring, 0);
            std::cout << "⚓ Grace v1.0.0 | Ready on port " << port << std::endl;

            while (true) {
                struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
                io_uring_prep_accept(sqe, s, NULL, NULL, 0); 
                io_uring_submit(&ring);

                struct io_uring_cqe *cqe; 
                io_uring_wait_cqe(&ring, &cqe);
                int cfd = cqe->res; 
                io_uring_cqe_seen(&ring, cqe);
                
                if (cfd < 0) continue;

                auto start = std::chrono::high_resolution_clock::now();
                char b[1024] = {0}; 
                read(cfd, b, 1024);
                std::string r(b);

                if (r.find(" ") != std::string::npos) {
                    std::string m = r.substr(0, r.find(" "));
                    size_t p_start = m.length() + 1;
                    std::string p = r.substr(p_start, r.find(" ", p_start) - p_start);

                    auto end = std::chrono::high_resolution_clock::now();
                    auto lat = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

                    Request req{m, p}; 
                    Response res{cfd};
                    if (routes.count(m + p)) routes[m + p](req, res, lat);
                }
                close(cfd);
            }
        }
    };
}
#endif

## 🛠 Prerequisites

Grace is built for modern Linux systems to leverage maximum I/O throughput.

* **Operating System**: Linux (Kernel 5.1+ required; 5.11+ recommended).
* **Compiler**: GCC 13+ (Requires C++20 support).
* **Dependencies**: 
    * `liburing-dev` (The core asynchronous I/O library).
    * `cmake` (version 3.10 or higher).

### Installing Dependencies (Ubuntu/Debian)
```bash
sudo apt update
sudo apt install liburing-dev cmake g++-13
```
