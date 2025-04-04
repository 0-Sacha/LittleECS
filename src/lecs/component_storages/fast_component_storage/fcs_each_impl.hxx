#pragma once

#include "fast_component_storage.hxx"

namespace lecs::detail {
    template <typename ComponentType>
        requires(TypeValidForComponentStorage<ComponentType>::value)
    template <typename Function>
    void FastComponentStorage<ComponentType>::foreach_storage(Function&& function)
        requires(ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF && ComponentStorageInfo<ComponentType>::SEND_ENTITIES_POOL_ON_EACH == false)
    {
        for (EntityId entity : alive_entities_container_) {
            ComponentType& component = get_entity_componenttype(entity);

            if constexpr (requires { function(entity, component); })
                function(entity, component);
            else if constexpr (requires { function(component); })
                function(component);
        }
    }

    template <typename ComponentType>
        requires(TypeValidForComponentStorage<ComponentType>::value)
    template <typename Function>
    void FastComponentStorage<ComponentType>::foreach_storage(Function&& function) const
        requires(ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF && ComponentStorageInfo<ComponentType>::SEND_ENTITIES_POOL_ON_EACH == false)
    {
        for (EntityId entity : alive_entities_container_) {
            const ComponentType& component = get_entity_componenttype(entity);

            if constexpr (requires { function(entity, component); })
                function(entity, component);
            else if constexpr (requires { function(component); })
                function(component);
        }
    }


    template <typename ComponentType>
        requires(TypeValidForComponentStorage<ComponentType>::value)
    template <typename Function>
    void FastComponentStorage<ComponentType>::foreach_storage(Function&& function, const auto& alive_entities_registry)
        requires(ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF == false && ComponentStorageInfo<ComponentType>::SEND_ENTITIES_POOL_ON_EACH)
    {
        for (EntityId entity : alive_entities_registry) {
            Index::IndexInfo indexinfo = get_entity_indexinfo(entity);

            if (indexinfo.index_of_page >= page_container_.size()) continue;
            if (page_container_[indexinfo.index_of_page] == nullptr) continue;

            ComponentType* component = page_container_[indexinfo.index_of_page]->get_component_at_indexptr(indexinfo.component_pageindex);

            if (component == nullptr) continue;

            if constexpr (requires { function(entity, *component); })
                function(entity, *component);
            else if constexpr (requires { function(*component); })
                function(*component);
        }
    }

    template <typename ComponentType>
        requires(TypeValidForComponentStorage<ComponentType>::value)
    template <typename Function>  // FuWnction = std::function<void(EntityId, const ComponentType&)>
    void FastComponentStorage<ComponentType>::foreach_storage(Function&& function, const auto& alive_entities_registry) const
        requires(ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF == false && ComponentStorageInfo<ComponentType>::SEND_ENTITIES_POOL_ON_EACH)
    {
        for (EntityId entity : alive_entities_registry) {
            Index::IndexInfo indexinfo = get_entity_indexinfo(entity);

            if (indexinfo.index_of_page >= page_container_.size()) continue;
            if (page_container_[indexinfo.index_of_page] == nullptr) continue;

            const ComponentType* component = page_container_[indexinfo.index_of_page]->get_component_at_indexptr(indexinfo.component_pageindex);

            if (component == nullptr) continue;

            if constexpr (requires { function(entity, *component); })
                function(entity, *component);
            else if constexpr (requires { function(*component); })
                function(*component);
        }
    }
}  // namespace lecs::detail
