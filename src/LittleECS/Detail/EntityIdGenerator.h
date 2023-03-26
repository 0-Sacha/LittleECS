#pragma once

#include "EntityId.h"

#include <queue>

namespace LittleECS::Detail
{

    class EntityIdGenerator
    {
    public:
        EntityId GetNewEntityId()
        {
            if (m_FreeEntityId.size() > 0)
            {
                EntityId res = m_FreeEntityId.front();
                m_FreeEntityId.pop();
                return res;
            }

            return m_CurrentIndex++;
        }

        void EntityIdDelete(EntityId entity)
        {
            typename EntityId::Type id = entity.Id;
            if (id == m_CurrentIndex - 1)
            {
                m_CurrentIndex--;
                return;
            }

            m_FreeEntityId.push(id);
        }

    protected:
        typename EntityId::Type m_CurrentIndex = EntityId::FIRST;
        std::queue<typename EntityId::Type> m_FreeEntityId;
    };

}
