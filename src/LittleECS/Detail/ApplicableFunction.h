#pragma once

#include <tuple>

namespace LittleECS::Detail
{
    template<typename Function, typename... Args>
    struct IsApplicable
    {
        static constexpr bool Value = std::is_invocable_v<Function, Args...>;
    };

    template<typename Function, typename... Args>
    struct IsApplicable<Function, std::tuple<Args...>>
    {
        static constexpr bool Value = std::is_invocable_v<Function, Args...>;
    };

    template<typename Function, typename... Args>
    struct IsApplicable<Function, const std::tuple<Args...>>
    {
        static constexpr bool Value = std::is_invocable_v<Function, Args...>;
    };
}
