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
    
    public:
        Type Value;
    };

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
        static TypeId m_NextGlobalTypeId;
    };
}
