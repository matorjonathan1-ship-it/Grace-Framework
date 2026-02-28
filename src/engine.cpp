#include <liburing.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <vector>

namespace grace {

class Engine {
    struct io_uring ring;
    int server_fd;

public:
    Engine() {
        // Initialize the Ring: 4096 depth for high throughput
        io_uring_queue_init(4096, &ring, 0);
    }

    ~Engine() {
        io_uring_queue_exit(&ring);
        close(server_fd);
    }

    void setup_server(int port) {
        server_fd = socket(AF_INET, SOCK_STREAM, 0);
        
        sockaddr_in addr = {};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = INADDR_ANY;

        bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));
        listen(server_fd, 1024);
    }

    // The 'Admiral's Submission'
    void submit_accept() {
        struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
        // This tells Linux: "Don't wait, just tell me when someone connects"
        io_uring_prep_accept(sqe, server_fd, nullptr, nullptr, 0);
        io_uring_submit(&ring);
    }
};

} // namespace grace
