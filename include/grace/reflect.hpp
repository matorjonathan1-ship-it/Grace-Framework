#ifndef GRACE_REFLECT_HPP
#define GRACE_REFLECT_HPP

#include <tuple>
#include <type_traits>

namespace grace {

/**
 * @brief The GRACE_REFLECT macro.
 * * Your vision for "Strict" reflection is implemented here.
 * Instead of the framework "guessing" (which is slow and risky), 
 * the user explicitly defines the fields to be exposed.
 */
#define GRACE_REFLECT(...) \
    /** Returns a tuple of references to the member variables */ \
    auto reflect_members() { \
        return std::tie(__VA_ARGS__); \
    } \
    /** A compile-time flag to verify this struct is 'Grace-Ready' */ \
    static constexpr bool is_grace_reflected = true;

/**
 * @brief A 'Concept' to prove a type is reflected at compile-time.
 * Part of the "Routing Theorem" philosophy.
 */
template <typename T>
concept Reflected = requires(T t) {
    { t.reflect_members() };
    T::is_grace_reflected == true;
};

} // namespace grace

#endif // GRACE_REFLECT_HPP
