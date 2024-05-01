#pragma once

#include "Registry.h"

namespace LECS
{
    // Function = std::function<void(EntityId, ComponentType& component)>
    template<typename ComponentType, typename Function>
    void Registry::ForEachUniqueComponent(Function&& function)
    {
        typename Detail::ComponentStorageInfo<ComponentType>::StorageType* componentStorage = GetComponentStorage<ComponentType>();
        if (componentStorage == nullptr)
            return;
        
        if constexpr (Detail::ComponentStorageInfo<ComponentType>::SEND_ENTITIES_POOL_ON_EACH == false)
        {
            componentStorage->ForEachStorage(std::forward<Function>(function));
        }
        else
        {
            componentStorage->ForEachStorage(std::forward<Function>(function), m_EntityIdGenerator.GetAlivesEntities());
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
            componentStorage->ForEachStorage(std::forward<Function>(function));
        }
        else
        {
            componentStorage->ForEachStorage(std::forward<Function>(function), m_EntityIdGenerator.GetAlivesEntities());
        }
    }   

    // Function = std::function<void(EntityId, ComponentTypes&... components)>
    template<typename... ComponentTypes, typename Function>
    void Registry::ForEachComponents(Function&& function)
    {
        if constexpr (sizeof...(ComponentTypes) == 1)
        {
            return ForEachUniqueComponent<ComponentTypes...>(std::forward<Function>(function));
        }
        else
        {
            BasicView<ComponentTypes...> view(*this);
            view.template ForEachComponents<ComponentTypes...>(std::forward<Function>(function));
        }
    }

    // Function = std::function<void(EntityId, RangeComponent& component, ComponentTypes&... components)>
    template<typename... ComponentTypes, typename Function>
    void Registry::ForEachComponents(Function&& function) const
    {
        if constexpr (sizeof...(ComponentTypes) == 1)
        {
            return ForEachUniqueComponent<ComponentTypes...>(std::forward<Function>(function));
        }
        else
        {
            BasicConstView<ComponentTypes...> view(*this);
            view.template ForEachComponents<ComponentTypes...>(std::forward<Function>(function));
        }
    }
}
