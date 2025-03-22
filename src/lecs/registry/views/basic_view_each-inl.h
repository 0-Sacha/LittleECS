#pragma once

#include "basic_view.h"
#include "basic_view_iterator.h"

namespace lecs
{
    template <typename... ViewComponentTypes>
    template <typename ComponentTypeEach>
    decltype(auto) BasicConstView<ViewComponentTypes...>::each_entities_with() const
    {
        const typename detail::ComponentStorageInfo<ComponentTypeEach>::StorageType* component_storage = get_component_storage_at<TypeIndex<ComponentTypeEach>::index>();

        LECS_ASSERT(component_storage, "Component storage can't be non referenced when getting iterators")
        
        if constexpr (detail::ComponentStorageInfo<ComponentTypeEach>::SEND_ENTITIES_POOL_ON_EACH == false)
            return detail::Iterable(component_storage->entities_iterator_begin(), component_storage->entities_iterator_end());
        else
            return detail::Iterable(
                component_storage->entities_iterator_begin(this->linked_registry_.get_entityid_generator().get_alives_entities()),
                component_storage->entities_iterator_end(this->linked_registry_.get_entityid_generator().get_alives_entities())
            );
    }

    template <typename... ViewComponentTypes>
    template <typename RangeComponent, typename... ComponentTypesEach>
    decltype(auto) BasicConstView<ViewComponentTypes...>::each_entities_with_all() const
    {
        if constexpr (sizeof...(ComponentTypesEach) == 0)
            return each_entities_with<RangeComponent>();
        else
        {
            auto ranged_component = this->each_entities_with<RangeComponent>();
            return detail::Iterable(
                detail::ViewEntitiesIterator<M_Type, decltype(ranged_component.begin()), decltype(ranged_component.end()), ComponentTypesEach...>(this, ranged_component.begin(), ranged_component.end()),
                detail::IterableEnd()
            );
        }
    }

    template <typename... ViewComponentTypes>
    template <typename ComponentTypeEach>
    decltype(auto) BasicConstView<ViewComponentTypes...>::each_unique_component() const
    {
        const typename detail::ComponentStorageInfo<ComponentTypeEach>::StorageType* component_storage = get_component_storage_at<TypeIndex<ComponentTypeEach>::index>();

        LECS_ASSERT(component_storage, "Component storage can't be non referenced when getting iterators")
        
        if constexpr (detail::ComponentStorageInfo<ComponentTypeEach>::SEND_ENTITIES_POOL_ON_EACH == false)
            return detail::Iterable(component_storage->entities_iterator_begin(), component_storage->entities_iterator_end());
        else
            return detail::Iterable(component_storage->entities_iterator_begin(this->linked_registry_.get_entityid_generator().get_alives_entities()),
                                    component_storage->entities_iterator_end(this->linked_registry_.get_entityid_generator().get_alives_entities()));
    }

    template <typename... ViewComponentTypes>
    template <typename RangeComponent, typename... ComponentTypesEach>
    decltype(auto) BasicConstView<ViewComponentTypes...>::each_components() const
    {
        auto ranged_component = each_entities_with<RangeComponent>();
        auto entities = detail::ViewEntitiesIterator<const M_Type, decltype(ranged_component.begin()), decltype(ranged_component.end()), ComponentTypesEach...>(this, ranged_component.begin(), ranged_component.end());
        
        return detail::Iterable(
            detail::ViewComponentsIterator<const M_Type, decltype(entities), detail::IterableEnd, false, RangeComponent, ComponentTypesEach...>(this, entities, detail::IterableEnd()),
            detail::IterableEnd()
        );
    }
}

namespace lecs
{
    template <typename... ViewComponentTypes>
    template <typename ComponentTypeEach>
    decltype(auto) BasicView<ViewComponentTypes...>::each_unique_component()
    {
        typename detail::ComponentStorageInfo<ComponentTypeEach>::StorageType* component_storage = get_component_storage_at<TypeIndex<ComponentTypeEach>::index>();

        LECS_ASSERT(component_storage, "Component storage can't be non referenced when getting iterators")
        
        if constexpr (detail::ComponentStorageInfo<ComponentTypeEach>::SEND_ENTITIES_POOL_ON_EACH == false)
            return detail::Iterable(component_storage->entities_iterator_begin(), component_storage->entities_iterator_end());
        else
            return detail::Iterable(component_storage->entities_iterator_begin(this->linked_registry_.get_entityid_generator().get_alives_entities()),
                                    component_storage->entities_iterator_end(this->linked_registry_.get_entityid_generator().get_alives_entities()));
    }
    template <typename... ViewComponentTypes>
    template <typename RangeComponent, typename... ComponentTypesEach>
    decltype(auto) BasicView<ViewComponentTypes...>::each_components()
    {
        auto ranged_component = this->template each_entities_with<RangeComponent>();
        auto entities = detail::ViewEntitiesIterator<M_Type, decltype(ranged_component.begin()), decltype(ranged_component.end()), ComponentTypesEach...>(this, ranged_component.begin(), ranged_component.end());
        
        return detail::Iterable(
            detail::ViewComponentsIterator<M_Type, decltype(entities), detail::IterableEnd, false, RangeComponent, ComponentTypesEach...>(this, entities, detail::IterableEnd()),
            detail::IterableEnd()
        );
    }
}
