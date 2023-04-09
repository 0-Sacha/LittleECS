#pragma once

#include "FastComponentStorage.h"

namespace LittleECS::Detail
{
    namespace Detail
    {
        template <typename FastComponentStorage, typename ContainerIterator>
        class FCSIteratorNoRef
        {
        public:
            using iterator_category = std::forward_iterator_tag;
            using difference_type   = std::ptrdiff_t;
            using value_type        = EntityId;
            using pointer           = const EntityId*;
            using reference         = EntityId;

        public:
            FCSIteratorNoRef(FastComponentStorage* fastComponentStorage, ContainerIterator containerIterator, ContainerIterator containerIteratorLast)
                : m_FastComponentStorage(fastComponentStorage)
                , m_ContainerIterator(containerIterator)
                , m_ContainerIteratorLast(containerIteratorLast)
            {}

        private:

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
                bool currentEntityValid = true;
                do
                {
                    m_ContainerIterator++;
                    currentEntityValid = m_FastComponentStorage->EntityHasThisComponent(*m_ContainerIterator);      
                } while(m_ContainerIterator != m_ContainerIteratorLast && currentEntityValid == false);

                if (currentEntityValid == false)
                {
                    m_FastComponentStorage = nullptr;
                }

                return *this;
            }

            FCSIteratorNoRef operator++(int) { FCSIteratorNoRef res(m_FastComponentStorage, m_ContainerIterator, m_ContainerIteratorLast); ++(*this); return res; }

            bool operator==(const FCSIteratorNoRef& rhs)
            {
                return m_FastComponentStorage != nullptr &&
                    m_FastComponentStorage == rhs.m_FastComponentStorage &&
                    m_ContainerIterator == rhs.m_ContainerIterator &&
                    m_ContainerIteratorLast == rhs.m_ContainerIteratorLast;
            }

            bool operator!=(const FCSIteratorNoRef& rhs)
            {
                return !(*this == rhs);
            }

        private:
            FastComponentStorage* m_FastComponentStorage;
            ContainerIterator m_ContainerIterator;
            ContainerIterator m_ContainerIteratorLast;
        };
    }

    template <typename ComponentType>
    decltype(auto) FastComponentStorage<ComponentType>::begin(const auto& registryAliveEntities)
    requires (ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF == false && ComponentStorageInfo<ComponentType>::SEND_ENTITIES_POOL_ON_EACH)
    {
        return Detail::FCSIteratorNoRef(*this, registryAliveEntities.cbegin(), registryAliveEntities.cend());
    }

    template <typename ComponentType>
    decltype(auto) FastComponentStorage<ComponentType>::end(const auto& registryAliveEntities)
    requires (ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF == false && ComponentStorageInfo<ComponentType>::SEND_ENTITIES_POOL_ON_EACH)
    {
        return Detail::FCSIteratorNoRef(nullptr, registryAliveEntities.cbegin(), registryAliveEntities.cend());
    }

    template <typename ComponentType>
    decltype(auto) FastComponentStorage<ComponentType>::cbegin(const auto& registryAliveEntities) const
    requires (ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF == false && ComponentStorageInfo<ComponentType>::SEND_ENTITIES_POOL_ON_EACH)
    {
        return Detail::FCSIteratorNoRef(nullptr, registryAliveEntities.cbegin(), registryAliveEntities.cend());
    }

    template <typename ComponentType>
    decltype(auto) FastComponentStorage<ComponentType>::cend(const auto& registryAliveEntities) const
    requires (ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF == false && ComponentStorageInfo<ComponentType>::SEND_ENTITIES_POOL_ON_EACH)
    {
        return Detail::FCSIteratorNoRef(nullptr, registryAliveEntities.cbegin(), registryAliveEntities.cend());
    }
}
