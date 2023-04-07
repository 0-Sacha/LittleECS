#pragma once

#include "CompressedComponentStorage.h"

namespace LittleECS::Detail
{
    template <typename ComponentType>
    template <typename Function> // Function = std::function<void(EntityId, ComponentType&)>
    void CompressedComponentStorage<ComponentType>::ForEachUniqueComponent(Function&& function)
    requires (ComponentStorageInfo<ComponentType>::SEND_ENTITIES_POOL_ON_EACH == false)
    {
        if constexpr (ComponentStorageInfo<ComponentType>::USE_MAP_VERSION == false)
        {
            if constexpr (ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF)
            {
                for (EntityId entity : m_EntityToComponent.GetAliveContainer())
                {
                    ComponentType& component = GetComponentOfEntity(entity);
                    function(entity, component);
                }
            }
            else if constexpr (ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF == false)
            {
                for (PageTypeRef& page : m_PageContainer)
                {
                    page->ForEach([&function, &page](Index::PageIndexOfComponent index){
                        ComponentType& component = page->GetComponentAtIndex(index);
                        EntityId entity = page->GetEntityIdAtIndex(index);
                        function(entity, component);
                    });
                }
            }
        }
        else if constexpr (ComponentStorageInfo<ComponentType>::USE_MAP_VERSION)
        {
            // EntityId::Type, Index::IndexInfo
            for (auto [entityIdType, indexInfo] : m_EntityToComponent.GetContainer())
            {
                EntityId entity = entityIdType;
                PageTypeRef& page = m_PageContainer[indexInfo.IndexOfPage];
                ComponentType& component = page->GetComponentAtIndex(indexInfo.PageIndexOfComponent);
                function(entity, component);
            }
        }
    }

    template <typename ComponentType>
    template <typename Function> // Function = std::function<void(EntityId, const ComponentType&)>
    void CompressedComponentStorage<ComponentType>::ForEachUniqueComponent(Function&& function) const
    requires (ComponentStorageInfo<ComponentType>::SEND_ENTITIES_POOL_ON_EACH == false)
    {
        if constexpr (ComponentStorageInfo<ComponentType>::USE_MAP_VERSION == false)
        {
            if constexpr (ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF)
            {
                for (EntityId entity : m_EntityToComponent.GetAliveContainer())
                {
                    const ComponentType& component = GetComponentOfEntity(entity);
                    function(entity, component);
                }
            }
            else if constexpr (ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF == false)
            {
                for (PageTypeRef& page : m_PageContainer)
                {
                    page->ForEach([&function, &page](Index::PageIndexOfComponent index){
                        const ComponentType& component = page->GetComponentAtIndex(index);
                        EntityId entity = page->GetEntityIdAtIndex(index);
                        function(entity, component);
                    });
                }
            }
        }
        else if constexpr (ComponentStorageInfo<ComponentType>::USE_MAP_VERSION)
        {
            for (auto [entityIdType, indexInfo] : m_EntityToComponent.GetContainer())
            {
                EntityId entity = entityIdType;
                PageTypeRef& page = m_PageContainer[indexInfo.IndexOfPage];
                const ComponentType& component = page->GetComponentAtIndex(indexInfo.PageIndexOfComponent);
                function(entity, component);
            }
        }
    }
}
