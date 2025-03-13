#pragma once

#include "BasicView.h"

#include "LittleECS/Registry/Registry.h"

namespace LECS
{
    template <typename... ViewComponentTypes>
    template <std::size_t I, typename Component, typename... ComponentRest>
    void BasicConstView<ViewComponentTypes...>::RefreshRegistryLink()
    {
        m_LinkToComponentContainer[I] = m_LinkedRegistry.GetComponentStorage<Component>();
        LECS_ASSERT(m_LinkToComponentContainer[I] != nullptr, "Can't create a view with an unreferenced storage");
        if constexpr (sizeof...(ComponentRest) > 0)
            RefreshRegistryLink<I + 1, ComponentRest...>();
    }

    // Function = std::function<void(EntityId, ComponentTypeEach& component)>
    template <typename... ViewComponentTypes>
    template <typename ComponentTypeEach, typename Function>
    void BasicConstView<ViewComponentTypes...>::ForEachUniqueComponent(Function&& function) const
    {
        if constexpr (Detail::ComponentStorageInfo<ComponentTypeEach>::SEND_ENTITIES_POOL_ON_EACH == false)
            GetComponentStorageAt<TypeIndex<ComponentTypeEach>::Index>()->ForEachStorage(function);
        else
            GetComponentStorageAt<TypeIndex<ComponentTypeEach>::Index>()->ForEachStorage(function, m_LinkedRegistry.GetEntityIdGenerator().GetAlivesEntities());
    }
}
