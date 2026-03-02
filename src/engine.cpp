#include <iostream>
#include <liburing.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

namespace grace {
    class Engine {
    public:
        void run() {
            int port = 8080;
            int server_fd = socket(AF_INET, SOCK_STREAM, 0);
            
            // Allow immediate reuse of the port
            int opt = 1;
            setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

            struct sockaddr_in addr;
            memset(&addr, 0, sizeof(addr));
            addr.sin_family = AF_INET;
            addr.sin_addr.s_addr = INADDR_ANY;
            addr.sin_port = htons(port);

            if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
                std::cerr << "❌ Bind failed. Is port 8080 busy?" << std::endl;
                return;
            }

            listen(server_fd, 128);
            std::cout << "⚓ Grace Engine ACTIVE & LISTENING on port " << port << "..." << std::endl;

            // Initialize io_uring
            struct io_uring ring;
            io_uring_queue_init(32, &ring, 0);

            while (true) {
                struct sockaddr_in client_addr;
                socklen_t client_len = sizeof(client_addr);
                
                // For this MVP, we accept the connection to prove the network is alive
                int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
                
                if (client_fd >= 0) {
                    const char* response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 18\r\n\r\nGrace is Offline.";
                    send(client_fd, response, strlen(response), 0);
                    close(client_fd);
                }
            }
            io_uring_queue_exit(&ring);
        }
    };
}
