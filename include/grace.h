#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <liburing.h>
#include <netinet/in.h>
#include <unistd.h>
#include <chrono>

namespace grace {
    struct Request {
        std::string method;
        std::string path;
    };

    struct Response {
        int fd;
        void send_json(std::string json, long long latency_us) {
            std::string h = "HTTP/1.1 200 OK\r\n"
                            "Content-Type: application/json\r\n"
                            "Server: Grace/1.0\r\n"
                            "X-Response-Time: " + std::to_string(latency_us) + "us\r\n"
                            "Content-Length: " + std::to_string(json.length()) + "\r\n"
                            "Connection: close\r\n\r\n" + json;
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
            int server_fd = socket(AF_INET, SOCK_STREAM, 0);
            int opt = 1;
            setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
            struct sockaddr_in addr = { .sin_family = AF_INET, .sin_port = htons(port), .sin_addr = {INADDR_ANY} };
            bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));
            ::listen(server_fd, 128);

            struct io_uring ring;
            io_uring_queue_init(256, &ring, 0);
            std::cout << "🚀 Grace v1.0.0 | Port: " << port << " | Engine: io_uring" << std::endl;

            while (true) {
                struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
                io_uring_prep_accept(sqe, server_fd, NULL, NULL, 0);
                io_uring_submit(&ring);
                struct io_uring_cqe *cqe;
                io_uring_wait_cqe(&ring, &cqe);
                int client_fd = cqe->res;
                io_uring_cqe_seen(&ring, cqe);

                if (client_fd >= 0) {
                    auto start = std::chrono::high_resolution_clock::now();
                    char buf[1024] = {0};
                    read(client_fd, buf, 1024);
                    std::string raw(buf);
                    
                    if (raw.find(" ") != std::string::npos) {
                        size_t m_end = raw.find(" ");
                        std::string method = raw.substr(0, m_end);
                        size_t p_end = raw.find(" ", m_end + 1);
                        std::string path = raw.substr(m_end + 1, p_end - (m_end + 1));

                        auto end = std::chrono::high_resolution_clock::now();
                        auto latency = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

                        Request req{method, path};
                        Response res{client_fd};

                        if (routes.count(method + path)) {
                            routes[method + path](req, res, latency);
                        }
                    }
                    close(client_fd);
                }
            }
        }
    };
}
