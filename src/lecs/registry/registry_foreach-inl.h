#pragma once

#include "registry.h"

namespace lecs {
    // Function = std::function<void(EntityId, ComponentType& component)>
    template <typename ComponentType, typename Function>
    void Registry::foreach_unique_component(Function&& function) {
        typename detail::ComponentStorageInfo<ComponentType>::StorageType* component_storage = get_component_storage<ComponentType>();
        if (component_storage == nullptr) return;

        if constexpr (detail::ComponentStorageInfo<ComponentType>::SEND_ENTITIES_POOL_ON_EACH == false) {
            component_storage->foreach_storage(std::forward<Function>(function));
        } else {
            component_storage->foreach_storage(std::forward<Function>(function), entityid_generator_.get_alives_entities());
        }
    }

    // Function = std::function<void(EntityId, ComponentType& component)>
    template <typename ComponentType, typename Function>
    void Registry::foreach_unique_component(Function&& function) const {
        const typename detail::ComponentStorageInfo<ComponentType>::StorageType* component_storage = get_component_storage<ComponentType>();
        if (component_storage == nullptr) return;

        if constexpr (detail::ComponentStorageInfo<ComponentType>::SEND_ENTITIES_POOL_ON_EACH == false) {
            component_storage->foreach_storage(std::forward<Function>(function));
        } else {
            component_storage->foreach_storage(std::forward<Function>(function), entityid_generator_.get_alives_entities());
        }
    }

    // Function = std::function<void(EntityId, ComponentTypes&... components)>
    template <typename... ComponentTypes, typename Function>
    void Registry::foreach_components(Function&& function) {
        if constexpr (sizeof...(ComponentTypes) == 1) {
            return foreach_unique_component<ComponentTypes...>(std::forward<Function>(function));
        } else {
            BasicView<ComponentTypes...> view(*this);
            view.template foreach_components<ComponentTypes...>(std::forward<Function>(function));
        }
    }

    // Function = std::function<void(EntityId, RangeComponent& component, ComponentTypes&... components)>
    template <typename... ComponentTypes, typename Function>
    void Registry::foreach_components(Function&& function) const {
        if constexpr (sizeof...(ComponentTypes) == 1) {
            return foreach_unique_component<ComponentTypes...>(std::forward<Function>(function));
        } else {
            BasicConstView<ComponentTypes...> view(*this);
            view.template foreach_components<ComponentTypes...>(std::forward<Function>(function));
        }
    }
}  // namespace lecs
