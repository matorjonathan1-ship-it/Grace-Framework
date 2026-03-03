#pragma once
#include <string>
#include <type_traits>

namespace grace {
    template<typename T>
    std::string serialize(const T& val) {
        if constexpr (std::is_same_v<std::decay_t<T>, std::string>) {
            return "\"" + val + "\"";
        } else if constexpr (std::is_arithmetic_v<std::decay_t<T>>) {
            return std::to_string(val);
        } else {
            return "\"{object}\""; // Fallback for complex types
        }
    }

    template<typename... Args>
    std::string to_json(Args&&... args) {
        std::string result = "{";
        int i = 0;
        ([&](auto& arg) {
            result += "\"field_" + std::to_string(i++) + "\":";
            result += serialize(arg);
            if (i < sizeof...(args)) result += ",";
        }(args), ...);
        result += "}";
        return result;
    }
}
