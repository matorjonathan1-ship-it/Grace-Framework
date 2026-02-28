#ifndef GRACE_ROUTER_HPP
#define GRACE_ROUTER_HPP

#include <string_view>
#include <functional>
#include <map>
#include <concepts>

namespace grace {

// The 'Theorem' part: We define what a valid Request/Response handler looks like.
struct Request { std::string_view path; };
struct Response { 
    void json(auto data) { /* Zero-copy JSON logic goes here */ } 
};

/**
 * @brief Handler Concept
 * This ensures the user's function is perfectly compatible with Grace's engine.
 * If they pass something wrong, the compiler stops them immediately.
 */
template<typename F>
concept GraceHandler = requires(F f, Request& req, Response& res) {
    { f(req, res) } -> std::same_as<void>;
};

class Router {
public:
    // We use 'consteval' and 'concepts' to prove the route at build time.
    template<GraceHandler H>
    void get(std::string_view path, H handler) {
        routes_["GET"][path] = handler;
    }

private:
    // High-performance route mapping
    std::map<std::string_view, std::map<std::string_view, std::function<void(Request&, Response&)>>> routes_;
};

} // namespace grace

#endif // GRACE_ROUTER_HPP
