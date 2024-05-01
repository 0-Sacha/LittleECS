#pragma once

#include "FastComponentStorage.h"

namespace LECS::Detail
{
    template <typename ComponentType>
    requires (TypeValidForComponentStorage<ComponentType>::Value)
    template <typename Function>
    void FastComponentStorage<ComponentType>::ForEachStorage(Function&& function)
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
    requires (TypeValidForComponentStorage<ComponentType>::Value)
    template <typename Function>
    void FastComponentStorage<ComponentType>::ForEachStorage(Function&& function) const
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
    requires (TypeValidForComponentStorage<ComponentType>::Value)
    template <typename Function>
    void FastComponentStorage<ComponentType>::ForEachStorage(Function&& function, const auto& registryAliveEntities)
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
    requires (TypeValidForComponentStorage<ComponentType>::Value)
    template <typename Function> // FuWnction = std::function<void(EntityId, const ComponentType&)>
    void FastComponentStorage<ComponentType>::ForEachStorage(Function&& function, const auto& registryAliveEntities) const
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
