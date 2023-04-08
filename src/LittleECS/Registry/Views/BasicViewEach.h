#pragma once

#include "BasicView.h"

namespace LittleECS
{
    // Function = std::function<void(EntityId, ComponentTypeEach& component)>
    template <typename... ViewComponentTypes>
    template <typename ComponentTypeEach, typename Function>
    void BasicView<ViewComponentTypes...>::ForEachUniqueComponent(Function&& function)
    {
        if constexpr (Detail::ComponentStorageInfo<ComponentTypeEach>::SEND_ENTITIES_POOL_ON_EACH == false)
            GetComponentStorageAt<TypeIndex<ComponentTypeEach>::Index>()->ForEachUniqueComponent(function);
        else
            GetComponentStorageAt<TypeIndex<ComponentTypeEach>::Index>()->ForEachUniqueComponent(function, m_LinkedRegistry.GetEntityIdGenerator().GetAlivesEntities());
    }

    // Function = std::function<void(EntityId, ComponentTypeEach& component)>
    template <typename... ViewComponentTypes>
    template <typename ComponentTypeEach, typename Function>
    void BasicView<ViewComponentTypes...>::ForEachUniqueComponent(Function&& function) const
    {
        if constexpr (Detail::ComponentStorageInfo<ComponentTypeEach>::SEND_ENTITIES_POOL_ON_EACH == false)
            GetComponentStorageAt<TypeIndex<ComponentTypeEach>::Index>()->ForEachUniqueComponent(function);
        else
            GetComponentStorageAt<TypeIndex<ComponentTypeEach>::Index>()->ForEachUniqueComponent(function, m_LinkedRegistry.GetEntityIdGenerator().GetAlivesEntities());
    }

    // Function = std::function<void(EntityId, ComponentTypeRanged& component, ComponentTypesEach&... components)>
    template <typename... ViewComponentTypes>
    template <typename ComponentTypeRanged, typename... ComponentTypesEach, typename Function>
    void BasicView<ViewComponentTypes...>::ForEachComponents(Function&& function)
    {
        if constexpr (sizeof...(ComponentTypesEach) == 0)
            return ForEachUniqueComponent<ComponentTypeRanged>(function);
    
        auto dispatchFunction = [&](EntityId entity, ComponentTypeRanged& componentRanged) {
                if (EntityHasAll<ComponentTypesEach...>(entity) == false)
                    return;
                
                if constexpr (std::is_invocable_v<Function, decltype(std::tuple_cat(std::tuple<EntityId>(entity), std::tuple<ComponentTypeRanged&>(componentRanged), GetComponentTuple<ComponentTypesEach...>(entity)))>)
                {
                    std::apply(function, std::tuple_cat(std::tuple<EntityId>(entity), std::tuple<ComponentTypeRanged&>(componentRanged), GetComponentTuple<ComponentTypesEach...>(entity)));
                }
                else if constexpr (std::is_invocable_v<Function, decltype(std::tuple_cat(std::tuple<ComponentTypeRanged&>(componentRanged), GetComponentTuple<ComponentTypesEach...>(entity)))>)
                {
                    std::apply(function, std::tuple_cat(std::tuple<ComponentTypeRanged&>(componentRanged), GetComponentTuple<ComponentTypesEach...>(entity)));
                }
            };

        return ForEachUniqueComponent<ComponentTypeRanged>(dispatchFunction);
    }

    // Function = std::function<void(EntityId, ComponentTypeRanged& component, ComponentTypesEach&... components)>
    template <typename... ViewComponentTypes>
    template <typename ComponentTypeRanged, typename... ComponentTypesEach, typename Function>
    void BasicView<ViewComponentTypes...>::ForEachComponents(Function&& function) const
    {
        if constexpr (sizeof...(ComponentTypesEach) == 0)
            return ForEachUniqueComponent<ComponentTypeRanged>(function);

        auto dispatchFunction = [&](EntityId entity, ComponentTypeRanged& componentRanged) {
                if (EntityHasAll<ComponentTypesEach...>(entity) == false)
                    return;
                
                if constexpr (requires {
                    std::apply(function, std::tuple_cat(std::tuple<EntityId>(entity), std::tuple<const ComponentTypeRanged&>(componentRanged), GetComponentTuple<ComponentTypesEach...>(entity)));
                })
                {
                    std::apply(function, std::tuple_cat(std::tuple<EntityId>(entity), std::tuple<const ComponentTypeRanged&>(componentRanged), GetComponentTuple<ComponentTypesEach...>(entity)));
                }
                else if constexpr (requires {
                    std::apply(function, std::tuple_cat(std::tuple<const ComponentTypeRanged&>(componentRanged), GetComponentTuple<ComponentTypesEach...>(entity)));
                })
                {
                    std::apply(function, std::tuple_cat(std::tuple<const ComponentTypeRanged&>(componentRanged), GetComponentTuple<ComponentTypesEach...>(entity)));
                }
            };

        return ForEachUniqueComponent<ComponentTypeRanged>(dispatchFunction);
    }
}
