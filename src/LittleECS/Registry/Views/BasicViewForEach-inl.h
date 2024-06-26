#pragma once

#include "BasicView.h"
#include "BasicViewIterator.h"

#include "LittleECS/Detail/ApplicableFunction.h"

namespace LECS
{
    // Function = std::function<void(EntityId, ComponentTypeRanged& component, ComponentTypesEach&... components)>
    template <typename... ViewComponentTypes>
    template <typename ComponentTypeRanged, typename... ComponentTypesEach, typename Function>
    void BasicConstView<ViewComponentTypes...>::ForEachComponents(Function&& function) const
    {
        if constexpr (sizeof...(ComponentTypesEach) == 0)
            return ForEachUniqueComponent<ComponentTypeRanged>(std::forward<Function>(function));

        auto dispatchFunction = [&](EntityId entity, const ComponentTypeRanged& componentRanged) {
                if (HasAll<ComponentTypesEach...>(entity) == false)
                    return;
                
                if constexpr (Detail::IsApplicable<Function, EntityId>::Value)
                {
                    std::apply(function, std::tuple<EntityId>(entity));
                }
                else if constexpr (Detail::IsApplicable<Function, EntityId, const ComponentTypeRanged&, const ComponentTypesEach&...>::Value)
                {
                    std::apply(function, std::tuple_cat(std::tuple<EntityId>(entity), std::tuple<const ComponentTypeRanged&>(componentRanged), GetAll<ComponentTypesEach...>(entity)));
                }
                else if constexpr (Detail::IsApplicable<Function, const ComponentTypeRanged&, const ComponentTypesEach&...>::Value)
                {
                    std::apply(function, std::tuple_cat(std::tuple<const ComponentTypeRanged&>(componentRanged), GetAll<ComponentTypesEach...>(entity)));
                }
            };

        return ForEachUniqueComponent<ComponentTypeRanged>(dispatchFunction);
    }
}

namespace LECS
{
    // Function = std::function<void(EntityId, ComponentTypeEach& component)>
    template <typename... ViewComponentTypes>
    template <typename ComponentTypeEach, typename Function>
    void BasicView<ViewComponentTypes...>::ForEachUniqueComponent(Function&& function)
    {
        if constexpr (Detail::ComponentStorageInfo<ComponentTypeEach>::SEND_ENTITIES_POOL_ON_EACH == false)
            GetComponentStorageAt<TypeIndex<ComponentTypeEach>::Index>()->ForEachStorage(function);
        else
            GetComponentStorageAt<TypeIndex<ComponentTypeEach>::Index>()->ForEachStorage(function, GetRegistry().GetEntityIdGenerator().GetAlivesEntities());
    }

    // Function = std::function<void(EntityId, ComponentTypeRanged& component, ComponentTypesEach&... components)>
    template <typename... ViewComponentTypes>
    template <typename ComponentTypeRanged, typename... ComponentTypesEach, typename Function>
    void BasicView<ViewComponentTypes...>::ForEachComponents(Function&& function)
    {
        if constexpr (sizeof...(ComponentTypesEach) == 0)
            return ForEachUniqueComponent<ComponentTypeRanged>(std::forward<Function>(function));
    
        auto dispatchFunction = [&](EntityId entity, ComponentTypeRanged& componentRanged) {
                if (HasAll<ComponentTypesEach...>(entity) == false)
                    return;
                
                if constexpr (Detail::IsApplicable<Function, EntityId>::Value)
                {
                    std::apply(function, std::tuple<EntityId>(entity));
                }
                else if constexpr (Detail::IsApplicable<Function, EntityId, ComponentTypeRanged&, ComponentTypesEach&...>::Value)
                {
                    std::apply(function, std::tuple_cat(std::tuple<EntityId>(entity), std::tuple<ComponentTypeRanged&>(componentRanged), GetAll<ComponentTypesEach...>(entity)));
                }
                else if constexpr (Detail::IsApplicable<Function, ComponentTypeRanged&, ComponentTypesEach&...>::Value)
                {
                    std::apply(function, std::tuple_cat(std::tuple<ComponentTypeRanged&>(componentRanged), GetAll<ComponentTypesEach...>(entity)));
                }
            };

        return ForEachUniqueComponent<ComponentTypeRanged>(dispatchFunction);
    }
}
