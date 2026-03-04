#include "grace/engine.hpp"
#include <iostream>
#include <liburing.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

namespace grace {
    void Engine::run(Router& router) {
        struct io_uring ring;
        io_uring_queue_init(32, &ring, 0);

        int server_fd = socket(AF_INET, SOCK_STREAM, 0);
        int opt = 1;
        setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        struct sockaddr_in addr = { .sin_family = AF_INET, .sin_port = htons(8080), .sin_addr = {INADDR_ANY} };
        bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));
        listen(server_fd, SOMAXCONN);

        std::cout << "⚓ Grace Engine sailing on port 8080..." << std::endl;

        struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
        io_uring_prep_multishot_accept(sqe, server_fd, NULL, NULL, 0);
        io_uring_submit(&ring);

        while (true) {
            struct io_uring_cqe *cqe;
            io_uring_wait_cqe(&ring, &cqe);

            if (cqe->res >= 0) {
                int client_fd = cqe->res;
                
                Request req{ .path = "/" }; // Static for now, parsing comes next!
                Response res;
                
                router.dispatch(req, res);
                
                std::string header = "HTTP/1.1 200 OK\r\nContent-Length: " + std::to_string(res.body.length()) + "\r\n\r\n";
                send(client_fd, header.c_str(), header.length(), 0);
                send(client_fd, res.body.c_str(), res.body.length(), 0);
                
                close(client_fd);
            }
            io_uring_cqe_seen(&ring, cqe);
        }
    }
}
