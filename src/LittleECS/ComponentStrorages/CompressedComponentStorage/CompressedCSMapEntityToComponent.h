#pragma once

#include "CompressedComponentStoragePage.h"

#include <unordered_map>

namespace LECS::Detail 
{
    class CompressedCSMapEntityToComponent
    {
    public:
        using Container = std::unordered_map<typename EntityId::Type, Index::IndexInfo>;

    private:
        Container m_Container;

    public:
        const Container& GetContainer()
        {
            return m_Container;
        }

        inline bool HasEntity(EntityId entity) const
        {
            return m_Container.contains(entity.Id);
        }

        inline Index::IndexInfo GetIndexInfoOfEntity(EntityId entity) const
        {
            LECS_ASSERT(HasEntity(entity) == true)
            return m_Container.at(entity.Id);
        }

        inline void AddIndexInfoForEntity(EntityId entity, Index::IndexInfo indexInfo)
        {
            LECS_ASSERT(HasEntity(entity) == false)
            m_Container[entity.Id] = indexInfo;
        }

        inline void RemoveIndexInfoForEntity(EntityId entity)
        {
            LECS_ASSERT(HasEntity(entity) == true)
            m_Container[entity.Id].SetInvalid();
        }
    };
}
