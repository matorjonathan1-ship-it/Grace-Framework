#pragma once
#include <concepts>
#include <type_traits>

namespace grace {
    // Basic concept to check if a type can be handled by our JSON logic
    template<typename T>
    concept Reflected = requires(T a) {
        { a } -> std::convertible_to<T>;
    };
}
