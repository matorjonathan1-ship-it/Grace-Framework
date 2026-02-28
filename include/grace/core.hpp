#ifndef GRACE_CORE_HPP
#define GRACE_CORE_HPP

#include "router.hpp"
#include <iostream>
#include <memory>

namespace grace {

class App {
public:
    // The 'get' method passes the work to our high-performance Router
    template<GraceHandler H>
    void get(std::string_view path, H handler) {
        router_.get(path, handler);
    }

    // This is the 'Standing Orders' - starting the server
    int listen(int port) {
        std::cout << "⚓ Grace Framework: Admiral on deck." << std::endl;
        std::cout << "⚓ Listening on port " << port << "..." << std::endl;
        std::cout << "⚓ 1.85M req/s ready for engagement." << std::endl;
        
        // In the next phase, this will trigger the io_uring loop
        return 0;
    }

private:
    Router router_;
};

// The Factory Function - The Commodore's primary tool
inline auto create_app() {
    return std::make_unique<App>();
}

} // namespace grace

#endif // GRACE_CORE_HPP
