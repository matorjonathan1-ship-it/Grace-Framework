#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <functional>
#include <string_view>
#include <concepts>

namespace grace {

struct Request { std::string path; };
struct Response { 
    std::string body;
    void json(auto data) { this->body = "{ \"status\": \"success\" }"; }
};

template<typename H>
concept GraceHandler = requires(H h, Request& req, Response& res) {
    { h(req, res) } -> std::same_as<void>;
};

class Router {
public:
    // We store handlers as type-erased functions
    using HandlerFunc = std::function<void(Request&, Response&)>;
    std::unordered_map<std::string, HandlerFunc> routes;

    template<GraceHandler H>
    void get(std::string path, H handler) {
        std::cout << "📍 Registered: " << path << std::endl;
        routes[path] = handler;
    }

    void dispatch(Request& req, Response& res) {
        if (routes.contains(req.path)) {
            routes[req.path](req, res);
        } else {
            res.body = "HTTP/1.1 404 Not Found\r\n\r\n404 - Theorem Not Found";
        }
    }
};

} // namespace grace
