#pragma once

#include <tuple>

namespace lecs::detail {
    template <typename Function, typename... Args>
    struct is_invocable {
        static constexpr bool value = std::is_invocable_v<Function, Args...>;
    };

    template <typename Function, typename... Args>
    struct is_invocable<Function, std::tuple<Args...>> {
        static constexpr bool value = std::is_invocable_v<Function, Args...>;
    };

    template <typename Function, typename... Args>
    struct is_invocable<Function, const std::tuple<Args...>> {
        static constexpr bool value = std::is_invocable_v<Function, Args...>;
    };
}  // namespace lecs::detail
