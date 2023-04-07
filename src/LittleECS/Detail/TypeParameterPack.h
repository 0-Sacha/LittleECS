#pragma once

#include <utility>

namespace LittleECS::Detail
{

    template <std::size_t I, typename... Types>
    struct GetType
    {
        using Type = void;
    };

    template <std::size_t I, typename CurrentType, typename... RestTypes>
    requires (I > 0)
    struct GetType<I, CurrentType, RestTypes...>
    {
        using Type = GetType<I - 1, RestTypes...>;
    };

    template <std::size_t I, typename CurrentType, typename... RestTypes>
    requires (I == 0)
    struct GetType<I, CurrentType, RestTypes...>
    {
        using Type = CurrentType;
    };

    template <std::size_t I, typename CurrentType>
    requires (I == 0)
    struct GetType<I, CurrentType>
    {
        using Type = CurrentType;
    };

}
