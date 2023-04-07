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
                res = m_FreeEntitiesId.extract(m_FreeEntitiesId.begin()).value();
            else
                res = m_CurrentIndex++;

            LECS_ASSERT(m_AliveEntitiesId.contains(res) == false, "Create an EntityId with an already assigned id");
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

            LECS_ASSERT(m_AliveEntitiesId.contains(id) == true, "Destroy an EntityId with an non assigned id");
            
            m_AliveEntitiesId.erase(id);
            m_FreeEntitiesId.insert(id);
        }

        const std::set<typename EntityId::Type>& GetAlivesEntities() const
        {
            return m_AliveEntitiesId;
        } 

    protected:
        typename EntityId::Type m_CurrentIndex = EntityId::FIRST;
        std::set<typename EntityId::Type> m_FreeEntitiesId;
        std::set<typename EntityId::Type> m_AliveEntitiesId;
    };

}
