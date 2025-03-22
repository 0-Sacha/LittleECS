#pragma once

#include "basic_view.hxx"
#include "basic_view_iterator.hxx"

#include "lecs/detail/is_invocable.hxx"

namespace lecs {
    // Function = std::function<void(EntityId, ComponentTypeRanged& component, ComponentTypesEach&... components)>
    template <typename... ViewComponentTypes>
    template <typename ComponentTypeRanged, typename... ComponentTypesEach, typename Function>
    void BasicConstView<ViewComponentTypes...>::foreach_components(Function&& function) const {
        if constexpr (sizeof...(ComponentTypesEach) == 0) return foreach_unique_component<ComponentTypeRanged>(std::forward<Function>(function));

        auto dispatchFunction = [&](EntityId entity, const ComponentTypeRanged& componentRanged) {
            if (has_all<ComponentTypesEach...>(entity) == false) return;

            if constexpr (detail::is_invocable<Function, EntityId>::value) {
                std::apply(function, std::tuple<EntityId>(entity));
            } else if constexpr (detail::is_invocable<Function, EntityId, const ComponentTypeRanged&, const ComponentTypesEach&...>::value) {
                std::apply(function, std::tuple_cat(std::tuple<EntityId>(entity), std::tuple<const ComponentTypeRanged&>(componentRanged), get_all<ComponentTypesEach...>(entity)));
            } else if constexpr (detail::is_invocable<Function, const ComponentTypeRanged&, const ComponentTypesEach&...>::value) {
                std::apply(function, std::tuple_cat(std::tuple<const ComponentTypeRanged&>(componentRanged), get_all<ComponentTypesEach...>(entity)));
            }
        };

        return foreach_unique_component<ComponentTypeRanged>(dispatchFunction);
    }
}  // namespace lecs

namespace lecs {
    // Function = std::function<void(EntityId, ComponentTypeEach& component)>
    template <typename... ViewComponentTypes>
    template <typename ComponentTypeEach, typename Function>
    void BasicView<ViewComponentTypes...>::foreach_unique_component(Function&& function) {
        if constexpr (detail::ComponentStorageInfo<ComponentTypeEach>::SEND_ENTITIES_POOL_ON_EACH == false)
            get_component_storage_at<TypeIndex<ComponentTypeEach>::index>()->foreach_storage(function);
        else
            get_component_storage_at<TypeIndex<ComponentTypeEach>::index>()->foreach_storage(function, GetRegistry().get_entityid_generator().get_alives_entities());
    }

    // Function = std::function<void(EntityId, ComponentTypeRanged& component, ComponentTypesEach&... components)>
    template <typename... ViewComponentTypes>
    template <typename ComponentTypeRanged, typename... ComponentTypesEach, typename Function>
    void BasicView<ViewComponentTypes...>::foreach_components(Function&& function) {
        if constexpr (sizeof...(ComponentTypesEach) == 0) return foreach_unique_component<ComponentTypeRanged>(std::forward<Function>(function));

        auto dispatchFunction = [&](EntityId entity, ComponentTypeRanged& componentRanged) {
            if (has_all<ComponentTypesEach...>(entity) == false) return;

            if constexpr (detail::is_invocable<Function, EntityId>::value) {
                std::apply(function, std::tuple<EntityId>(entity));
            } else if constexpr (detail::is_invocable<Function, EntityId, ComponentTypeRanged&, ComponentTypesEach&...>::value) {
                std::apply(function, std::tuple_cat(std::tuple<EntityId>(entity), std::tuple<ComponentTypeRanged&>(componentRanged), get_all<ComponentTypesEach...>(entity)));
            } else if constexpr (detail::is_invocable<Function, ComponentTypeRanged&, ComponentTypesEach&...>::value) {
                std::apply(function, std::tuple_cat(std::tuple<ComponentTypeRanged&>(componentRanged), get_all<ComponentTypesEach...>(entity)));
            }
        };

        return foreach_unique_component<ComponentTypeRanged>(dispatchFunction);
    }
}  // namespace lecs
