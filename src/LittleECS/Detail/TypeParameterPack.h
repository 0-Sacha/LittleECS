#pragma once

#include <utility>
#include <type_traits>
#include <limits>

namespace LittleECS::Detail
{

    template <std::size_t I, typename... Types>
    struct GetTypeAt
    {
        using Type = void;
    };

    template <std::size_t I, typename CurrentType, typename... RestTypes>
    requires (I > 0)
    struct GetTypeAt<I, CurrentType, RestTypes...>
    {
        using Type = typename GetTypeAt<I - 1, RestTypes...>::Type;
    };

    template <std::size_t I, typename CurrentType, typename... RestTypes>
    requires (I == 0)
    struct GetTypeAt<I, CurrentType, RestTypes...>
    {
        using Type = CurrentType;
    };

    template <std::size_t I, typename CurrentType>
    requires (I == 0)
    struct GetTypeAt<I, CurrentType>
    {
        using Type = CurrentType;
    };


    template <typename TypeSerach, std::size_t I, typename... Types>
    struct GetTypeIndex
    {
        static constexpr std::size_t Index = std::numeric_limits<std::size_t>::max();
    };

    template <typename TypeSerach, std::size_t I, typename CurrentType, typename... RestTypes>
    requires (std::is_same_v<TypeSerach, CurrentType> == false)
    struct GetTypeIndex<TypeSerach, I, CurrentType, RestTypes...>
    {
        static constexpr std::size_t Index = GetTypeIndex<TypeSerach, I + 1, RestTypes...>::Index;
    };

    template <typename TypeSerach, std::size_t I, typename CurrentType, typename... RestTypes>
    requires (std::is_same_v<TypeSerach, CurrentType> == true)
    struct GetTypeIndex<TypeSerach, I, CurrentType, RestTypes...>
    {
        static constexpr std::size_t Index = I;
    };

    template <typename TypeSerach, std::size_t I, typename CurrentType>
    requires (std::is_same_v<TypeSerach, CurrentType>)
    struct GetTypeIndex<TypeSerach, I, CurrentType>
    {
        static constexpr std::size_t Index = I;
    };

}
