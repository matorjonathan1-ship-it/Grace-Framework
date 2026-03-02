#ifndef GRACE_HPP
#define GRACE_HPP

#include <iostream>
#include <vector>
#include <functional>
#include <liburing.h>
#include <netinet/in.h>
#include <unistd.h>
#include <chrono>
#include <thread>
#include <cstring>

namespace grace {
    struct Request { const char* method; const char* path; };
    
    // Connection State Machine
    enum { EVENT_ACCEPT, EVENT_READ, EVENT_WRITE };
    struct conn_info {
        int fd;
        int type;
        char buffer[512];
    };

    struct Route { std::string path; std::function<void(Request&, int, long long)> cb; };

    class App {
        std::vector<Route> get_routes;
    public:
        void get(std::string path, std::function<void(Request&, int, long long)> cb) {
            get_routes.push_back({path, cb});
        }

        void add_accept(struct io_uring *ring, int sock_fd) {
            struct io_uring_sqe *sqe = io_uring_get_sqe(ring);
            conn_info *conn = new conn_info{sock_fd, EVENT_ACCEPT};
            io_uring_prep_accept(sqe, sock_fd, NULL, NULL, 0);
            io_uring_sqe_set_data(sqe, conn);
        }

        void worker(int port) {
            int s = socket(AF_INET, SOCK_STREAM, 0);
            int opt = 1;
            setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
            setsockopt(s, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
            
            struct sockaddr_in a = { .sin_family = AF_INET, .sin_port = htons(port), .sin_addr = {INADDR_ANY} };
            bind(s, (struct sockaddr*)&a, sizeof(a));
            ::listen(s, 8192);

            struct io_uring ring;
            io_uring_queue_init(4096, &ring, 0);

            add_accept(&ring, s);

            while (true) {
                struct io_uring_cqe *cqe;
                io_uring_submit_and_wait(&ring, 1);
                
                unsigned head;
                unsigned count = 0;
                io_uring_for_each_cqe(&ring, head, cqe) {
                    count++;
                    conn_info *conn = (conn_info *)cqe->user_data;

                    if (conn->type == EVENT_ACCEPT) {
                        int cfd = cqe->res;
                        add_accept(&ring, s); // Keep accepting
                        
                        if (cfd >= 0) {
                            struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
                            conn_info *new_conn = new conn_info{cfd, EVENT_READ};
                            io_uring_prep_recv(sqe, cfd, new_conn->buffer, 512, 0);
                            io_uring_sqe_set_data(sqe, new_conn);
                        }
                        delete conn;
                    } else if (conn->type == EVENT_READ) {
                        int bytes = cqe->res;
                        if (bytes > 0) {
                            auto start = std::chrono::high_resolution_clock::now();
                            conn->buffer[bytes] = '\0';
                            char *m = strtok(conn->buffer, " ");
                            char *p = strtok(NULL, " ");
                            
                            if (m && p) {
                                auto end = std::chrono::high_resolution_clock::now();
                                auto lat = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
                                Request req{m, p};
                                
                                bool found = false;
                                for (auto& route : get_routes) {
                                    if (route.path == p) {
                                        route.cb(req, conn->fd, lat);
                                        found = true; break;
                                    }
                                }
                                if (!found) {
                                    const char* nf = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
                                    send(conn->fd, nf, strlen(nf), 0);
                                }
                            }
                        }
                        close(conn->fd);
                        delete conn;
                    }
                }
                io_uring_cq_advance(&ring, count);
                io_uring_submit(&ring);
            }
        }

        void listen(int port) {
            unsigned int cores = std::thread::hardware_concurrency();
            std::cout << "🚀 GRACE ELITE v1.0.0 | " << cores << " Workers | Zero-Syscall Mode" << std::endl;
            std::vector<std::thread> threads;
            for (unsigned int i = 0; i < cores; ++i) threads.emplace_back(&App::worker, this, port);
            for (auto& t : threads) t.join();
        }
    };
}
#endif
