#pragma once

#include "Core.h"
#include <limits>

namespace LittleECS
{

    class EntityId
    {
    public:
        using Type = std::size_t;
        static constexpr Type NON_VALID = std::numeric_limits<Type>::max();
        static constexpr Type FIRST = 0;

    public:
        inline constexpr EntityId(Type id = FIRST)
            : Id(id)
        {}

        inline constexpr operator Type () const
        {
            return Id;
        }

        inline constexpr EntityId& operator=(Type id)
        {
            Id = id;
            return *this;
        } 

    public:
        Type Id;
    };

    inline bool constexpr operator==(EntityId lhs, EntityId rhs)
    {
        return lhs.Id == rhs.Id;
    }

    inline bool constexpr operator==(EntityId lhs, EntityId::Type rhs)
    {
        return lhs.Id == rhs;
    }

    inline bool constexpr operator==(EntityId::Type lhs, EntityId rhs)
    {
        return lhs == rhs.Id;
    }
    
}

namespace ProjectCore::FMT
{
	template<typename FormatterContext>
	struct FormatterType<LittleECS::EntityId, FormatterContext> {
		static void Format(LittleECS::EntityId typeId, FormatterContext& context)
        {
			FormatterType<Detail::ForwardAsUInt<typename LittleECS::EntityId::Type>, FormatterContext>::Format(typeId, context);
		}
	};
}
