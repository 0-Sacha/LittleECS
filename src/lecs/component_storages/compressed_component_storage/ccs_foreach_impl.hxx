#pragma once

#include "compressed_component_storage.h"

namespace lecs::detail {
    template <typename ComponentType>
        requires(TypeValidForComponentStorage<ComponentType>::value)
    template <typename Function, typename ComponentConstness>  // Function = std::function<void(EntityId, ComponentType&)>
    void CompressedComponentStorage<ComponentType>::foreach_storage_impl(Function&& function)
        requires(ComponentStorageInfo<ComponentType>::SEND_ENTITIES_POOL_ON_EACH == false)
    {
        if constexpr (ComponentStorageInfo<ComponentType>::USE_MAP_VERSION == false) {
            if constexpr (ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF) {
                for (EntityId::Type entity : entity_to_component_.get_alive_container()) {
                    ComponentConstness& component = get_entity_componenttype(entity);

                    if constexpr (requires { function(entity, component); })
                        function(entity, component);
                    else if constexpr (requires { function(component); })
                        function(component);
                }
            } else if constexpr (ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF == false) {
                for (PageTypeRef& page : page_container_) {
                    page->foreach_page([&function, &page](Index::ComponentPageIndex index) {
                        EntityId            entity    = page->get_entityid_at_index(index);
                        ComponentConstness& component = page->get_component_at_index(index);

                        if constexpr (requires { function(entity, component); })
                            function(entity, component);
                        else if constexpr (requires { function(component); })
                            function(component);
                    });
                }
            }
        } else if constexpr (ComponentStorageInfo<ComponentType>::USE_MAP_VERSION) {
            // EntityId::Type, Index::IndexInfo
            for (auto [entityid_type, indexinfo] : entity_to_component_.get_container()) {
                EntityId            entity    = entityid_type;
                PageTypeRef&        page      = page_container_[indexinfo.index_of_page];
                ComponentConstness& component = page->get_component_at_index(indexinfo.component_pageindex);

                if constexpr (requires { function(entity, component); })
                    function(entity, component);
                else if constexpr (requires { function(component); })
                    function(component);
            }
        }
    }
}  // namespace lecs::detail
