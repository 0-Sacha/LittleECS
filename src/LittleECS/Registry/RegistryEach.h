#pragma once

#include "Registry.h"

namespace LittleECS
{
    // Function = std::function<void(EntityId)>
    template<typename Function>
    void Registry::ForEachEntities(Function&& function)
    {
        for (EntityId entity : m_EntityIdGenerator.GetAlivesEntities())
            function(entity);
    }

    // Function = std::function<void(EntityId, ComponentType& component)>
    template<typename ComponentType, typename Function>
    void Registry::ForEachUniqueComponent(Function&& function)
    {
        typename Detail::ComponentStorageInfo<ComponentType>::StorageType* componentStorage = GetComponentStorage<ComponentType>();
        if (componentStorage == nullptr)
            return;
        
        if constexpr (Detail::ComponentStorageInfo<ComponentType>::SEND_ENTITIES_POOL_ON_EACH == false)
        {
            componentStorage->ForEachUniqueComponent(function);
        }
        else
        {
            componentStorage->ForEachUniqueComponent(function, m_EntityIdGenerator.GetAlivesEntities());
        }
    }

    // Function = std::function<void(EntityId, ComponentType& component)>
    template<typename ComponentType, typename Function>
    void Registry::ForEachUniqueComponent(Function&& function) const
    {
        const typename Detail::ComponentStorageInfo<ComponentType>::StorageType* componentStorage = GetComponentStorage<ComponentType>();
        if (componentStorage == nullptr)
            return;
        
        if constexpr (Detail::ComponentStorageInfo<ComponentType>::SEND_ENTITIES_POOL_ON_EACH == false)
        {
            componentStorage->ForEachUniqueComponent(function);
        }
        else
        {
            componentStorage->ForEachUniqueComponent(function, m_EntityIdGenerator.GetAlivesEntities());
        }
    }   

    // Function = std::function<void(EntityId, ComponentTypes&... components)>
    template<typename... ComponentTypes, typename Function>
    void Registry::ForEachComponents(Function&& function)
    {
        if constexpr (sizeof...(ComponentTypes) == 1)
        {
            return ForEachUniqueComponent<ComponentTypes...>(function);
        }
        else
        {
            BasicView<ComponentTypes...> view(*this);
            view.ForEach<ComponentTypes...>(function);
        }
    }

    // Function = std::function<void(EntityId, RangeComponent& component, ComponentTypes&... components)>
    template<typename... ComponentTypes, typename Function>
    void Registry::ForEachComponents(Function&& function) const
    {
        if constexpr (sizeof...(ComponentTypes) == 1)
        {
            return ForEachUniqueComponent<ComponentTypes...>(function);
        }
        else
        {
            BasicView<ComponentTypes...> view(*this);
            view.ForEach<ComponentTypes...>(function);
        }
    }
}
