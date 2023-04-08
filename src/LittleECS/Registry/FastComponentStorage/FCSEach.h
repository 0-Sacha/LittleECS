#pragma once

#include "FastComponentStorage.h"

namespace LittleECS::Detail
{
    template <typename ComponentType>
    template <typename Function>
    void FastComponentStorage<ComponentType>::ForEachUniqueComponent(Function&& function)
    requires (ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF && ComponentStorageInfo<ComponentType>::SEND_ENTITIES_POOL_ON_EACH == false)
    {
        for (EntityId entity : m_AliveEntitiesContainer)
        {
            ComponentType& component = GetComponentOfEntity(entity);
            
            if constexpr (requires { function(entity, component); })
                function(entity, component);
            else if constexpr (requires { function(component); })
                function(component);
        }
    }

    template <typename ComponentType>
    template <typename Function>
    void FastComponentStorage<ComponentType>::ForEachUniqueComponent(Function&& function) const
    requires (ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF && ComponentStorageInfo<ComponentType>::SEND_ENTITIES_POOL_ON_EACH == false)
    {
        for (EntityId entity : m_AliveEntitiesContainer)
        {
            const ComponentType& component = GetComponentOfEntity(entity);
            
            if constexpr (requires { function(entity, component); })
                function(entity, component);
            else if constexpr (requires { function(component); })
                function(component);
        }
    }


    template <typename ComponentType>
    template <typename Function, typename Container>
    void FastComponentStorage<ComponentType>::ForEachUniqueComponent(Function&& function, const Container& registryAliveEntities)
    requires (ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF == false && ComponentStorageInfo<ComponentType>::SEND_ENTITIES_POOL_ON_EACH)
    {
        for (EntityId entity : registryAliveEntities)
        {
            Index::IndexInfo indexInfo = GetIndexInfoOfEntity(entity);

            if (indexInfo.IndexOfPage >= m_PageContainer.size())
                continue;
            if (m_PageContainer[indexInfo.IndexOfPage] == nullptr)
                continue;

            ComponentType* component = m_PageContainer[indexInfo.IndexOfPage]->GetComponentAtIndexPtr(indexInfo.PageIndexOfComponent);

            if (component == nullptr)
                continue;

            if constexpr (requires { function(entity, *component); })
                function(entity, *component);
            else if constexpr (requires { function(*component); })
                function(*component);
        }
    }

    template <typename ComponentType>
    template <typename Function, typename Container> // Function = std::function<void(EntityId, const ComponentType&)>
    void FastComponentStorage<ComponentType>::ForEachUniqueComponent(Function&& function, const Container& registryAliveEntities) const
    requires (ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF == false && ComponentStorageInfo<ComponentType>::SEND_ENTITIES_POOL_ON_EACH)
    {
        for (EntityId entity : registryAliveEntities)
        {
            Index::IndexInfo indexInfo = GetIndexInfoOfEntity(entity);

            if (indexInfo.IndexOfPage >= m_PageContainer.size())
                continue;
            if (m_PageContainer[indexInfo.IndexOfPage] == nullptr)
                continue;

            const ComponentType* component = m_PageContainer[indexInfo.IndexOfPage]->GetComponentAtIndexPtr(indexInfo.PageIndexOfComponent);

            if (component == nullptr)
                continue;
            
            if constexpr (requires { function(entity, *component); })
                function(entity, *component);
            else if constexpr (requires { function(*component); })
                function(*component);
        }
    }
}
