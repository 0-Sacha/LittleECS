#pragma once

#include "FastComponentStorage.h"

#include "LittleECS/Detail/Iterable.h"

namespace LittleECS::Detail
{
	template <typename FastComponentStorage, typename ContainerIterator>
	class FCSIteratorNoRef
	{
	public:
		using iterator_category = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = EntityId;
		using pointer = const EntityId*;
		using reference = EntityId;

	public:
		FCSIteratorNoRef(const FastComponentStorage* fastComponentStorage, ContainerIterator containerIterator, ContainerIterator containerIteratorLast)
			: m_FastComponentStorage(fastComponentStorage)
			, m_ContainerIterator(containerIterator)
			, m_ContainerIteratorLast(containerIteratorLast)
		{}

	public:
		reference operator*() const
		{
			return m_ContainerIterator.operator*();
		}
		pointer operator->()
		{
			return m_ContainerIterator.operator->();
		}

		FCSIteratorNoRef& operator++()
		{
			bool currentEntityValid = false;
			do
			{
				++m_ContainerIterator;
				if (m_ContainerIterator == m_ContainerIteratorLast)
					break;
				
				EntityId entity = *m_ContainerIterator;
				currentEntityValid = m_FastComponentStorage->EntityHasThisComponent(entity);
			} while (currentEntityValid == false);

			if (currentEntityValid == false)
			{
				m_FastComponentStorage = nullptr;
			}

			return *this;
		}

		FCSIteratorNoRef operator++(int)
		{
			FCSIteratorNoRef res = this;
			++(*this);
			return res;
		}

		bool operator==(const FCSIteratorNoRef& rhs) const
		{
#ifdef LECS_DEBUG
			bool complexResult = m_FastComponentStorage != nullptr &&
				m_FastComponentStorage == rhs.m_FastComponentStorage &&
				m_ContainerIterator == rhs.m_ContainerIterator &&
				m_ContainerIteratorLast == rhs.m_ContainerIteratorLast;
			bool simpleResult = m_FastComponentStorage != nullptr;

			LECS_ASSERT(simpleResult == complexResult, "Operator== for Iterator is wrong")

			return simpleResult;
#else
			return m_FastComponentStorage != nullptr;
#endif
		}

		bool operator!=(const FCSIteratorNoRef& rhs) const { return !(*this == rhs); }

		bool operator==(IterableEnd rhs) { return m_FastComponentStorage == nullptr; }
		bool operator!=(IterableEnd rhs) { return !(*this == rhs); }

	private:
		const FastComponentStorage* m_FastComponentStorage;
		ContainerIterator m_ContainerIterator;
		ContainerIterator m_ContainerIteratorLast;
	};

    template <typename ComponentType>
    decltype(auto) FastComponentStorage<ComponentType>::cbegin(const auto& registryAliveEntities) const
    requires (ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF == false && ComponentStorageInfo<ComponentType>::SEND_ENTITIES_POOL_ON_EACH)
    {
        return FCSIteratorNoRef(this, registryAliveEntities.cbegin(), registryAliveEntities.cend());
    }

    template <typename ComponentType>
    decltype(auto) FastComponentStorage<ComponentType>::cend(const auto&) const
    requires (ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF == false && ComponentStorageInfo<ComponentType>::SEND_ENTITIES_POOL_ON_EACH)
    {
		return IterableEnd();
    }
}
