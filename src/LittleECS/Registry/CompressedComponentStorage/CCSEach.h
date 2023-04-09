#pragma once

#include "CompressedComponentStorage.h"

namespace LittleECS::Detail
{
    template <typename ComponentType>
    template <typename Function, typename ComponentConstness> // Function = std::function<void(EntityId, ComponentType&)>
    void CompressedComponentStorage<ComponentType>::ForEachStorageImpl(Function&& function)
    requires (ComponentStorageInfo<ComponentType>::SEND_ENTITIES_POOL_ON_EACH == false)
    {
        if constexpr (ComponentStorageInfo<ComponentType>::USE_MAP_VERSION == false)
        {
            if constexpr (ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF)
            {
                for (EntityId::Type entity : m_EntityToComponent.GetAliveContainer())
                {
                    ComponentConstness& component = GetComponentOfEntity(entity);

                    if constexpr (requires { function(entity, component); })
                        function(entity, component);
                    else if constexpr (requires { function(component); })
                        function(component);
                }
            }
            else if constexpr (ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF == false)
            {
                for (PageTypeRef& page : m_PageContainer)
                {
                    page->ForEachPage([&function, &page](Index::PageIndexOfComponent index){
                        EntityId entity = page->GetEntityIdAtIndex(index);
                        ComponentConstness& component = page->GetComponentAtIndex(index);

                        if constexpr (requires { function(entity, component); })
                            function(entity, component);
                        else if constexpr (requires { function(component); })
                            function(component);
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
                ComponentConstness& component = page->GetComponentAtIndex(indexInfo.PageIndexOfComponent);

                if constexpr (requires { function(entity, component); })
                    function(entity, component);
                else if constexpr (requires { function(component); })
                    function(component);
            }
        }
    }
}
