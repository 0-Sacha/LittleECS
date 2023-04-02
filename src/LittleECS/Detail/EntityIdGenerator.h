#pragma once

#include "EntityId.h"

#include <queue>
#include <set>

namespace LittleECS::Detail
{

    class EntityIdGenerator
    {
    public:
        EntityId GetNewEntityId()
        {
            typename EntityId::Type res;

            if (m_FreeEntitiesId.size() > 0)
            {
                res = m_FreeEntitiesId.front();
                m_FreeEntitiesId.pop();
            }
            else
            {
                res = m_CurrentIndex++;
            }

            m_AliveEntitiesId.insert(res);
            return res;
        }

        void EntityIdDelete(EntityId entity)
        {
            typename EntityId::Type id = entity.Id;
            if (id == m_CurrentIndex - 1)
            {
                m_CurrentIndex--;
                return;
            }

            m_FreeEntitiesId.push(id);
            m_AliveEntitiesId.erase(id);
        }

        const std::set<typename EntityId::Type>& GetAlivesEntities()
        {
            return m_AliveEntitiesId;
        } 

    protected:
        typename EntityId::Type m_CurrentIndex = EntityId::FIRST;
        std::queue<typename EntityId::Type> m_FreeEntitiesId;
        std::set<typename EntityId::Type> m_AliveEntitiesId;
    };

}
