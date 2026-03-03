#include "reflect.hpp"
#ifndef GRACE_ROUTER_HPP
#define GRACE_ROUTER_HPP

#include "json.hpp" // Connect the voice to the brain
#include <string_view>
#include <functional>
#include <map>
#include <concepts>

namespace grace {

struct Request { 
    std::string_view path; 
    std::string_view method;
};

/**
 * @brief The Commodore's Response Object
 * This is where the "Strict" philosophy meets the web.
 */
struct Response {
    std::string body;
    int status = 200;

    /**
     * @brief Serialize reflected data
     * This method is only available if the data has been 'Proved' via GRACE_REFLECT.
     */
    template <Reflected T>
    void json(T& data) {
        this->body = grace::to_json(data);
    }

    void send(std::string_view raw_text) {
        this->body = raw_text;
    }
};

/**
 * @brief Handler Concept
 * Ensures the developer's function matches the Admiral's specifications.
 */
template<typename F>
concept GraceHandler = requires(F f, Request& req, Response& res) {
    { f(req, res) } -> std::same_as<void>;
};

class Router {
public:
    template<GraceHandler H>
    void get(std::string_view path, H handler) {
        routes_["GET"][path] = handler;
    }

private:
    // High-performance route mapping: Method -> Path -> Handler
    std::map<std::string_view, std::map<std::string_view, std::function<void(Request&, Response&)>>> routes_;
};

} // namespace grace

#endif // GRACE_ROUTER_HPP
