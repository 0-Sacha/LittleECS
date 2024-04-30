#pragma once

#include "Registry.h"

#include "LittleECS/Detail/Iterable.h"

namespace LECS
{
	template<typename ComponentType>
	decltype(auto) Registry::EachEntitiesWith()
	{
		typename Detail::ComponentStorageInfo<ComponentType>::StorageType* componentStorage = GetComponentStorage<ComponentType>();

        LECS_ASSERT(componentStorage, "Component storage can't be non referenced when getting iterators")
		
		if constexpr (Detail::ComponentStorageInfo<ComponentType>::SEND_ENTITIES_POOL_ON_EACH == false)
            return Detail::Iterable(componentStorage->EntitiesIteratorBegin(), componentStorage->EntitiesIteratorEnd());
		else
            return Detail::Iterable(componentStorage->EntitiesIteratorBegin(this->m_EntityIdGenerator.GetAlivesEntities()),
                					componentStorage->EntitiesIteratorEnd(this->m_EntityIdGenerator.GetAlivesEntities())
        		   );
	}
	template<typename ComponentType>
	decltype(auto) Registry::EachEntitiesWith() const
	{
        const typename Detail::ComponentStorageInfo<ComponentType>::StorageType* componentStorage = GetComponentStorage<ComponentType>();

        LECS_ASSERT(componentStorage, "Component storage can't be non referenced when getting iterators")
		
		if constexpr (Detail::ComponentStorageInfo<ComponentType>::SEND_ENTITIES_POOL_ON_EACH == false)
            return Detail::Iterable(componentStorage->EntitiesIteratorBegin(), componentStorage->EntitiesIteratorEnd());
		else
            return Detail::Iterable(componentStorage->EntitiesIteratorBegin(this->m_EntityIdGenerator.GetAlivesEntities()),
                                    componentStorage->EntitiesIteratorEnd(this->m_EntityIdGenerator.GetAlivesEntities())
				   );
	}
}
