#pragma once

#include "Registry.h"

#include "LittleECS/Detail/Iterable.h"

namespace LittleECS
{
	template<typename ComponentType>
	decltype(auto) Registry::EachEntitiesUniqueComponent()
	{
		typename Detail::ComponentStorageInfo<ComponentType>::StorageType* componentStorage = GetComponentStorage<ComponentType>();

        LECS_ASSERT(componentStorage, "Component storage can't be non referenced when getting iterators")
		
		if constexpr (Detail::ComponentStorageInfo<ComponentType>::SEND_ENTITIES_POOL_ON_EACH == false)
		{
            return Detail::Iterable([&componentStorage](){
                return componentStorage->cbegin();
            }, [&componentStorage](){
                return componentStorage->cend();
            });
		}
		else
		{
            return Detail::Iterable([&componentStorage, this](){
                return componentStorage->cbegin(this->m_EntityIdGenerator.GetAlivesEntities());
            }, [&componentStorage, this](){
                return componentStorage->cend(this->m_EntityIdGenerator.GetAlivesEntities());
            });
		}
	}
	template<typename ComponentType>
	decltype(auto) Registry::EachEntitiesUniqueComponent() const
	{
        const typename Detail::ComponentStorageInfo<ComponentType>::StorageType* componentStorage = GetComponentStorage<ComponentType>();

        LECS_ASSERT(componentStorage, "Component storage can't be non referenced when getting iterators")
		
		if constexpr (Detail::ComponentStorageInfo<ComponentType>::SEND_ENTITIES_POOL_ON_EACH == false)
		{
            return Detail::Iterable([&componentStorage](){
                return componentStorage->cbegin();
            }, [&componentStorage](){
                return componentStorage->cend();
            });
		}
		else
		{
            return Detail::Iterable([&componentStorage, this](){
                return componentStorage->cbegin(this->m_EntityIdGenerator.GetAlivesEntities());
            }, [&componentStorage, this](){
                return componentStorage->cend(this->m_EntityIdGenerator.GetAlivesEntities());
            });
		}
	}
}
