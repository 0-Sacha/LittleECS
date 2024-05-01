#pragma once

#include "BasicView.h"
#include "BasicViewIterator.h"

namespace LECS
{
    template <typename... ViewComponentTypes>
    template <typename ComponentTypeEach>
    decltype(auto) BasicConstView<ViewComponentTypes...>::EachEntitiesWith() const
    {
        const typename Detail::ComponentStorageInfo<ComponentTypeEach>::StorageType* componentStorage = GetComponentStorageAt<TypeIndex<ComponentTypeEach>::Index>();

        LECS_ASSERT(componentStorage, "Component storage can't be non referenced when getting iterators")
        
        if constexpr (Detail::ComponentStorageInfo<ComponentTypeEach>::SEND_ENTITIES_POOL_ON_EACH == false)
            return Detail::Iterable(componentStorage->EntitiesIteratorBegin(), componentStorage->EntitiesIteratorEnd());
        else
            return Detail::Iterable(
                componentStorage->EntitiesIteratorBegin(this->m_LinkedRegistry.GetEntityIdGenerator().GetAlivesEntities()),
                componentStorage->EntitiesIteratorEnd(this->m_LinkedRegistry.GetEntityIdGenerator().GetAlivesEntities())
            );
    }

    template <typename... ViewComponentTypes>
    template <typename RangeComponent, typename... ComponentTypesEach>
    decltype(auto) BasicConstView<ViewComponentTypes...>::EachEntitiesWithAll() const
    {
        if constexpr (sizeof...(ComponentTypesEach) == 0)
            return EachEntitiesWith<RangeComponent>();
        else
        {
            auto rangedComponent = this->EachEntitiesWith<RangeComponent>();
            return Detail::Iterable(
                Detail::ViewEntitiesIterator<M_Type, decltype(rangedComponent.begin()), decltype(rangedComponent.end()), ComponentTypesEach...>(this, rangedComponent.begin(), rangedComponent.end()),
                Detail::IterableEnd()
            );
        }
    }

    template <typename... ViewComponentTypes>
    template <typename ComponentTypeEach>
    decltype(auto) BasicConstView<ViewComponentTypes...>::EachUniqueComponent() const
    {
        const typename Detail::ComponentStorageInfo<ComponentTypeEach>::StorageType* componentStorage = GetComponentStorageAt<TypeIndex<ComponentTypeEach>::Index>();

        LECS_ASSERT(componentStorage, "Component storage can't be non referenced when getting iterators")
        
        if constexpr (Detail::ComponentStorageInfo<ComponentTypeEach>::SEND_ENTITIES_POOL_ON_EACH == false)
            return Detail::Iterable(componentStorage->EntitiesIteratorBegin(), componentStorage->EntitiesIteratorEnd());
        else
            return Detail::Iterable(componentStorage->EntitiesIteratorBegin(this->m_LinkedRegistry.GetEntityIdGenerator().GetAlivesEntities()),
                                    componentStorage->EntitiesIteratorEnd(this->m_LinkedRegistry.GetEntityIdGenerator().GetAlivesEntities()));
    }

    template <typename... ViewComponentTypes>
    template <typename RangeComponent, typename... ComponentTypesEach>
    decltype(auto) BasicConstView<ViewComponentTypes...>::EachComponents() const
    {
        auto rangedComponent = EachEntitiesWith<RangeComponent>();
        auto entities = Detail::ViewEntitiesIterator<const M_Type, decltype(rangedComponent.begin()), decltype(rangedComponent.end()), ComponentTypesEach...>(this, rangedComponent.begin(), rangedComponent.end());
        
        return Detail::Iterable(
            Detail::ViewComponentsIterator<const M_Type, decltype(entities), Detail::IterableEnd, false, RangeComponent, ComponentTypesEach...>(this, entities, Detail::IterableEnd()),
            Detail::IterableEnd()
        );
    }
}

namespace LECS
{
    template <typename... ViewComponentTypes>
    template <typename ComponentTypeEach>
    decltype(auto) BasicView<ViewComponentTypes...>::EachUniqueComponent()
    {
        typename Detail::ComponentStorageInfo<ComponentTypeEach>::StorageType* componentStorage = GetComponentStorageAt<TypeIndex<ComponentTypeEach>::Index>();

        LECS_ASSERT(componentStorage, "Component storage can't be non referenced when getting iterators")
        
        if constexpr (Detail::ComponentStorageInfo<ComponentTypeEach>::SEND_ENTITIES_POOL_ON_EACH == false)
            return Detail::Iterable(componentStorage->EntitiesIteratorBegin(), componentStorage->EntitiesIteratorEnd());
        else
            return Detail::Iterable(componentStorage->EntitiesIteratorBegin(this->m_LinkedRegistry.GetEntityIdGenerator().GetAlivesEntities()),
                                    componentStorage->EntitiesIteratorEnd(this->m_LinkedRegistry.GetEntityIdGenerator().GetAlivesEntities()));
    }
    template <typename... ViewComponentTypes>
    template <typename RangeComponent, typename... ComponentTypesEach>
    decltype(auto) BasicView<ViewComponentTypes...>::EachComponents()
    {
        auto rangedComponent = this->template EachEntitiesWith<RangeComponent>();
        auto entities = Detail::ViewEntitiesIterator<M_Type, decltype(rangedComponent.begin()), decltype(rangedComponent.end()), ComponentTypesEach...>(this, rangedComponent.begin(), rangedComponent.end());
        
        return Detail::Iterable(
            Detail::ViewComponentsIterator<M_Type, decltype(entities), Detail::IterableEnd, false, RangeComponent, ComponentTypesEach...>(this, entities, Detail::IterableEnd()),
            Detail::IterableEnd()
        );
    }
}
