#pragma once

#include <utility>
#include <type_traits>
#include <limits>

namespace lecs::detail
{
    template <std::size_t I, typename... Types>
    struct get_type_at
    {
        using type = void;
    };

    template <std::size_t I, typename CurrentType, typename... RestTypes>
    requires (I > 0)
    struct get_type_at<I, CurrentType, RestTypes...>
    {
        using type = typename get_type_at<I - 1, RestTypes...>::type;
    };

    template <std::size_t I, typename CurrentType, typename... RestTypes>
    requires (I == 0)
    struct get_type_at<I, CurrentType, RestTypes...>
    {
        using type = CurrentType;
    };

    template <std::size_t I, typename CurrentType>
    requires (I == 0)
    struct get_type_at<I, CurrentType>
    {
        using type = CurrentType;
    };


    template <typename TypeSearch, std::size_t I, typename... Types>
    struct get_type_index
    {
        static constexpr std::size_t index = std::numeric_limits<std::size_t>::max();
    };

    template <typename TypeSearch, std::size_t I, typename CurrentType, typename... RestTypes>
    requires (std::is_same_v<TypeSearch, CurrentType> == false)
    struct get_type_index<TypeSearch, I, CurrentType, RestTypes...>
    {
        static constexpr std::size_t index = get_type_index<TypeSearch, I + 1, RestTypes...>::index;
    };


    template <typename T1, typename T2>
    struct same_cvref
    {
        static constexpr bool value = std::is_same_v<std::remove_cvref_t<T1>, std::remove_cvref_t<T2>>;
    };

    template <typename TypeSearch, std::size_t I, typename CurrentType, typename... RestTypes>
    requires (same_cvref<TypeSearch, CurrentType>::value)
    struct get_type_index<TypeSearch, I, CurrentType, RestTypes...>
    {
        static constexpr std::size_t index = I;
    };

    template <typename TypeSearch, std::size_t I, typename CurrentType>
    requires (same_cvref<TypeSearch, CurrentType>::value)
    struct get_type_index<TypeSearch, I, CurrentType>
    {
        static constexpr std::size_t index = I;
    };

}
