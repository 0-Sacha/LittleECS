#pragma once

#include "basic_view.hxx"

#include "lecs/registry/registry.hxx"

namespace lecs {
    template <typename... ViewComponentTypes>
    template <std::size_t I, typename Component, typename... ComponentRest>
    void BasicConstView<ViewComponentTypes...>::refresh_registry_link() {
        link_to_component_container_[I] = linked_registry_.get_component_storage<Component>();
        LECS_ASSERT(link_to_component_container_[I] != nullptr, "Can't create a view with an unreferenced storage");
        if constexpr (sizeof...(ComponentRest) > 0) refresh_registry_link<I + 1, ComponentRest...>();
    }

    // Function = std::function<void(EntityId, ComponentTypeEach& component)>
    template <typename... ViewComponentTypes>
    template <typename ComponentTypeEach, typename Function>
    void BasicConstView<ViewComponentTypes...>::foreach_unique_component(Function&& function) const {
        if constexpr (detail::ComponentStorageInfo<ComponentTypeEach>::SEND_ENTITIES_POOL_ON_EACH == false)
            get_component_storage_at<TypeIndex<ComponentTypeEach>::index>()->foreach_storage(function);
        else
            get_component_storage_at<TypeIndex<ComponentTypeEach>::index>()->foreach_storage(function, linked_registry_.get_entityid_generator().get_alives_entities());
    }
}  // namespace lecs
