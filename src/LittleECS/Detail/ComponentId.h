#pragma once

#include "Core.h"
#include <limits>

namespace LittleECS
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
	struct FormatterType<LittleECS::ComponentId, FormatterContext> {
		static void Format(LittleECS::ComponentId typeId, FormatterContext& context)
        {
			FormatterType<Detail::ForwardAsUInt<typename LittleECS::ComponentId::Type>, FormatterContext>::Format(typeId, context);
		}
	};
}
