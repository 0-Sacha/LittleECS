#pragma once

#include "Core.h"

#include "ComponentId.h"

namespace LittleECS::Detail
{
    class GlobalComponentIdGenerator
    {
    public:
        template <typename T>
        static ComponentId GetTypeId()
        {
            static ComponentId id = ComponentId::NON_VALID;
            if (id != ComponentId::NON_VALID)
                return id;
            return id = Next();
        }

    private:
        static ComponentId Next()
        {
            ComponentId res = m_NextGlobalComponentId;
            m_NextGlobalComponentId.Id++;
            return res;
        }

    private:
        static inline ComponentId m_NextGlobalComponentId{ ComponentId::FIRST };
    };
}
