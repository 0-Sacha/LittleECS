#pragma once

#include "LittleECS/Core/Core.h"

#include <limits>

namespace LECS
{
    struct ComponentId
    {
    public:
        using Type = std::size_t;
        static constexpr Type INVALID = std::numeric_limits<Type>::max();
        static constexpr Type FIRST = 0;
    
    public:
        inline constexpr ComponentId(Type id = FIRST)
            : Id(id)
        {}

        inline constexpr operator Type () const
        {
            return Id;
        }

        inline constexpr ComponentId& operator=(Type id)
        {
            Id = id;
            return *this;
        } 

    public:
        Type Id;
    };

    inline bool constexpr operator==(ComponentId lhs, ComponentId rhs)
    {
        return lhs.Id == rhs.Id;
    }

    inline bool constexpr operator==(ComponentId lhs, ComponentId::Type rhs)
    {
        return lhs.Id == rhs;
    }

    inline bool constexpr operator==(ComponentId::Type lhs, ComponentId rhs)
    {
        return lhs == rhs.Id;
    }
}

namespace ProjectCore::FMT
{
    template<typename FormatterContext>
    struct FormatterType<LECS::ComponentId, FormatterContext> {
        static void Format(LECS::ComponentId typeId, FormatterContext& context)
        {
            FormatterType<Detail::ForwardAsUInt<typename LECS::ComponentId::Type>, FormatterContext>::Format(typeId, context);
        }
    };
}
