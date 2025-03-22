#pragma once

#include "registry.h"

#include "lecs/detail/iterable.h"

namespace lecs
{
    template<typename ComponentType>
    decltype(auto) Registry::each_entities_with()
    {
        typename detail::ComponentStorageInfo<ComponentType>::StorageType* component_storage = get_component_storage<ComponentType>();

        LECS_ASSERT(component_storage, "Component storage can't be non referenced when getting iterators")
        
        if constexpr (detail::ComponentStorageInfo<ComponentType>::SEND_ENTITIES_POOL_ON_EACH == false)
            return detail::Iterable(component_storage->entities_iterator_begin(), component_storage->entities_iterator_end());
        else
            return detail::Iterable(component_storage->entities_iterator_begin(this->entityid_generator_.get_alives_entities()),
                                    component_storage->entities_iterator_end(this->entityid_generator_.get_alives_entities())
                   );
    }
    template<typename ComponentType>
    decltype(auto) Registry::each_entities_with() const
    {
        const typename detail::ComponentStorageInfo<ComponentType>::StorageType* component_storage = get_component_storage<ComponentType>();

        LECS_ASSERT(component_storage, "Component storage can't be non referenced when getting iterators")
        
        if constexpr (detail::ComponentStorageInfo<ComponentType>::SEND_ENTITIES_POOL_ON_EACH == false)
            return detail::Iterable(component_storage->entities_iterator_begin(), component_storage->entities_iterator_end());
        else
            return detail::Iterable(component_storage->entities_iterator_begin(this->entityid_generator_.get_alives_entities()),
                                    component_storage->entities_iterator_end(this->entityid_generator_.get_alives_entities())
                   );
    }
}
