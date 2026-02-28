#ifndef GRACE_JSON_HPP
#define GRACE_JSON_HPP

#include "reflect.hpp"
#include <string>
#include <tuple>
#include <type_traits>

namespace grace {

/**
 * @brief The Commodore's High-Speed Serializer
 * Uses a fold expression to build the string without requiring 
 * heavy external formatting libraries.
 */
template <Reflected T>
std::string serialize(T& obj) {
    std::string result = "{";
    auto members = obj.reflect_members();

    std::apply([&result](auto&&... args) {
        int i = 0;
        ((
            result += (i == 0 ? "" : ","),
            result += "\"field_" + std::to_string(i) + "\":",
            if constexpr (std::is_same_v<std::decay_t<decltype(args)>, std::string>) {
                result += "\"" + args + "\"";
            } else {
                result += std::to_string(args);
            }
            , ++i
        ), ...);
    }, members);

    result += "}";
    return result;
}

} // namespace grace

#endif // GRACE_JSON_HPP
