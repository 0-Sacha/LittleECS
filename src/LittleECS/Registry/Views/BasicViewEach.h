#pragma once

#include "BasicView.h"

namespace LittleECS
{
    // Function = std::function<void(EntityId, ViewComponentTypesEach&... components)>
    template <typename... ViewComponentTypes>
    template <typename ComponentTypeEach, typename... ComponentTypesEach, typename Function>
    void BasicView<ViewComponentTypes...>::ForEach(Function&& function)
    {
        if constexpr (sizeof...(ComponentTypesEach) == 0)
        {
			if constexpr (Detail::ComponentStorageInfo<ComponentTypeEach>::SEND_ENTITIES_POOL_ON_EACH == false)
			{
                GetComponentStorageAt<TypeIndex<ComponentTypeEach>::Index>()->ForEachUniqueComponent(function);
			}
			else
			{
                GetComponentStorageAt<TypeIndex<ComponentTypeEach>::Index>()->ForEachUniqueComponent(function, m_LinkedRegistry.GetEntityIdGenerator().GetAlivesEntities());
			}
        }
        else
        {
            auto dispatchFunction = [&](EntityId entity, ComponentTypeEach& componentRanged){
                    if (EntityHasAll<ComponentTypesEach...>(entity) == false)
                        return;
                    std::apply(function, std::tuple_cat(std::tuple<EntityId>(entity), std::tuple<ComponentTypeEach&>(componentRanged), GetComponentTuple<ComponentTypesEach...>(entity)));
                };

            if constexpr (Detail::ComponentStorageInfo<ComponentTypeEach>::SEND_ENTITIES_POOL_ON_EACH == false)
            {
                GetComponentStorageAt<TypeIndex<ComponentTypeEach>::Index>()->ForEachUniqueComponent(dispatchFunction);
            }
            else
            {
                GetComponentStorageAt<TypeIndex<ComponentTypeEach>::Index>()->ForEachUniqueComponent(dispatchFunction, m_LinkedRegistry.GetEntityIdGenerator().GetAlivesEntities());
            }
        }
    }

    // Function = std::function<void(EntityId, ViewComponentTypesEach&... components)>
    template <typename... ViewComponentTypes>
    template <typename ComponentTypeEach, typename... ComponentTypesEach, typename Function>
    void BasicView<ViewComponentTypes...>::ForEach(Function&& function) const
    {
        if constexpr (sizeof...(ComponentTypesEach) == 0)
        {
			if constexpr (Detail::ComponentStorageInfo<ComponentTypeEach>::SEND_ENTITIES_POOL_ON_EACH == false)
			{
                GetComponentStorageAt<TypeIndex<ComponentTypeEach>::Index>()->ForEachUniqueComponent(function);
			}
			else
			{
                GetComponentStorageAt<TypeIndex<ComponentTypeEach>::Index>()->ForEachUniqueComponent(function, m_LinkedRegistry.GetEntityIdGenerator().GetAlivesEntities());
			}
        }
        else
        {
            auto dispatchFunction = [&](EntityId entity, const ComponentTypeEach& componentRanged){
                    if (EntityHasAll<ComponentTypesEach...>(entity) == false)
                        return;
                    std::apply(function, std::tuple_cat(std::tuple<EntityId>(entity), std::tuple<const ComponentTypeEach&>(componentRanged), GetComponentTuple<ComponentTypesEach...>(entity)));
                };

            if constexpr (Detail::ComponentStorageInfo<ComponentTypeEach>::SEND_ENTITIES_POOL_ON_EACH == false)
            {
                GetComponentStorageAt<TypeIndex<ComponentTypeEach>::Index>()->ForEachUniqueComponent(dispatchFunction);
            }
            else
            {
                GetComponentStorageAt<TypeIndex<ComponentTypeEach>::Index>()->ForEachUniqueComponent(dispatchFunction, m_LinkedRegistry.GetEntityIdGenerator().GetAlivesEntities());
            }
        }
    }
}
