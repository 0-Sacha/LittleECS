#pragma once

#include "Core.h"
#include <limits>

namespace LittleECS::Detail
{
    struct TypeId
    {
    public:
        using Type = std::uint32_t;
        static constexpr Type NON_VALID = std::numeric_limits<Type>::max();
        static constexpr Type FIRST = 0;
    
    public:
        inline constexpr TypeId(Type value = FIRST)
            : Value(value)
        {}

        inline operator Type ()
        {
            return Value;
        }

        inline TypeId& operator=(Type value)
        {
            Value = value;
            return *this;
        } 

    public:
        Type Value;
    };

    inline bool operator==(TypeId lhs, TypeId rhs)
    {
        return lhs.Value == rhs.Value;
    }

    inline bool operator==(TypeId lhs, TypeId::Type rhs)
    {
        return lhs.Value == rhs;
    }

    inline bool operator==(TypeId::Type lhs, TypeId rhs)
    {
        return lhs == rhs.Value;
    }

    class GlobalTypeIdGenerator
    {
    public:
        template <typename T>
        static TypeId GetTypeId()
        {
            static TypeId id = TypeId::NON_VALID;
            if (id != TypeId::NON_VALID)
                return id;
            return id = Next();
        }

    private:
        static TypeId Next()
        {
            TypeId res = m_NextGlobalTypeId;
            m_NextGlobalTypeId.Value++;
            return res;
        }

    private:
        static inline TypeId m_NextGlobalTypeId{ TypeId::FIRST };
    };
}

namespace ProjectCore::FMT
{
	template<typename FormatterContext>
	struct FormatterType<LittleECS::Detail::TypeId, FormatterContext> {
		static void Format(LittleECS::Detail::TypeId typeId, FormatterContext& context)
        {
			FormatterType<Detail::ForwardAsUInt<typename LittleECS::Detail::TypeId::Type>, FormatterContext>::Format(typeId, context);
		}
	};
}
