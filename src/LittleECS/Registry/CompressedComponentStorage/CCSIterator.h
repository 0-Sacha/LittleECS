#pragma once

#include "CompressedComponentStorage.h"

namespace LittleECS::Detail
{
     namespace Detail
    {
        template <typename CompressedComponentStorage>
        class CCSIteratorNoRefNoMap
        {
        public:
            using iterator_category = std::forward_iterator_tag;
            using difference_type   = std::ptrdiff_t;
            using value_type        = EntityId;
            using pointer           = const EntityId*;
            using reference         = EntityId;

        public:
            CCSIteratorNoRefNoMap(CompressedComponentStorage* compressedComponentStorage, std::size_t currentIndexOfPage = 0, std::size_t currentPageIndexOfCurrent = 0)
                : m_CompressedComponentStorage(compressedComponentStorage)
                , m_CurrentIndexOfPage(currentIndexOfPage)
                , m_CurrentPageIndexOfCurrent(currentPageIndexOfCurrent)
            {
                if (currentIndexOfPage == 0 && currentPageIndexOfCurrent == 0)
                {
                    if (CurrentEntityIsValid())
                    {
                        operator++();
                    }
                }

                LECS_ASSERT(CurrentEntityIsValid(), "Invalid Iterator")
            }

        private:
            bool CurrentEntityIsValid()
            {
                if (m_CurrentIndexOfPage >= m_CompressedComponentStorage->GetPageContainer().size())
                    return false;

                if (m_CurrentPageIndexOfCurrent >= CompressedComponentStorage::PAGE_SIZE)
                    return false;
            
                return m_CompressedComponentStorage->GetPageContainer()[m_CurrentIndexOfPage].HasComponentAtIndex(m_CurrentPageIndexOfCurrent)
            }

            EntityId* GetCurrentEntity()
            {
                LECS_ASSERT(CurrentEntityIsValid(), "Invalid Iterator")
                return &m_CompressedComponentStorage->GetPageContainer()[m_CurrentIndexOfPage].GetEntityIdAtIndex(m_CurrentPageIndexOfCurrent);
            }

        public:
            reference operator*() const
            {
                return m_ContainerIterator.operator*();
            }
            pointer operator->()
            {
                return &(operator*());
            }

            CCSIteratorNoRefNoMap& operator++()
            {
                bool found = false;
                while (m_CurrentIndexOfPage < m_CompressedComponentStorage->GetPageContainer().size())
                {
                    Index::PageIndexOfComponent nextValidIndex = m_CompressedComponentStorage->GetPageContainer()[m_CurrentIndexOfPage].GetNextValidIndex(m_CurrentPageIndexOfCurrent);
                    if (m_CurrentPageIndexOfCurrent < CompressedComponentStorage::PAGE_SIZE)
                    {
                        found = true;
                        m_CurrentPageIndexOfCurrent = CompressedComponentStorage::PAGE_SIZE;
                        m_CurrentIndexOfPage = m_CompressedComponentStorage->GetPageContainer().size();
                    }
                    ++m_CurrentIndexOfPage;
                }

                if (found == false)
                {
                    m_CompressedComponentStorage = nullptr;
                }

                return *this;
            }

            CCSIteratorNoRefNoMap operator++(int) { CCSIteratorNoRefNoMap res(m_CompressedComponentStorage, m_CurrentIndexOfPage, m_CurrentPageIndexOfCurrent); ++(*this); return res; }

            bool operator==(const FCSIteratorNoRef& rhs)
            {
                return m_CompressedComponentStorage != nullptr &&
                    m_CompressedComponentStorage == rhs.m_CompressedComponentStorage &&
                    m_CurrentIndexOfPage == rhs.m_CurrentIndexOfPage &&
                    m_CurrentPageIndexOfCurrent == rhs.m_CurrentPageIndexOfCurrent;
            }

            bool operator!=(const FCSIteratorNoRef& rhs)
            {
                return !(*this == rhs);
            }

        private:
            CompressedComponentStorage* m_CompressedComponentStorage;
            std::size_t m_CurrentIndexOfPage;
            std::size_t m_CurrentPageIndexOfCurrent;
        };

        template <typename SubIterator>
        class CCSIteratorNoRefMap
        {
        public:
            using iterator_category = SubIterator::iterator_category;
            using difference_type   = SubIterator::difference_type;
            using value_type        = EntityId;
            using pointer           = const EntityId*;
            using reference         = EntityId;

        public:
            CCSIteratorNoRefMap(SubIterator mapRefIterator)
                : m_MapRefIterator(mapRefIterator)
            {}

        public:
            reference operator*() const
            {
                return m_MapRefIterator->second;
            }
            pointer operator->()
            {
                return &m_MapRefIterator->second;
            }

            CCSIteratorNoRefMap& operator++()
            {
                return ++m_MapRefIterator;
            }

            CCSIteratorNoRefMap& operator++()
            {
                return m_MapRefIterator++;
            }

            bool operator==(const CCSIteratorNoRefMap& rhs)
            {
                return m_MapRefIterator == rhs.m_MapRefIterator;
            }

            bool operator!=(const CCSIteratorNoRefMap& rhs)
            {
                return m_MapRefIterator != rhs.m_MapRefIterator;
            }

        private:
            SubIterator m_MapRefIterator;
        };
    }

    template <typename ComponentType>
    decltype(auto) CompressedComponentStorage<ComponentType>::begin()
    {
        if constexpr (ComponentStorageInfo<ComponentType>::USE_MAP_VERSION == false)
        {
            if constexpr (ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF)
                return m_EntityToComponent.GetAliveContainer().begin();
            else if constexpr (ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF == false)
                return CCSIteratorNoRefNoMap(*this);
        }
        else if constexpr (ComponentStorageInfo<ComponentType>::USE_MAP_VERSION)
            return CCSIteratorNoRefMap(m_EntityToComponent.GetContainer().begin());
    }

    template <typename ComponentType>
    decltype(auto) CompressedComponentStorage<ComponentType>::end()
    {
       if constexpr (ComponentStorageInfo<ComponentType>::USE_MAP_VERSION == false)
        {
            if constexpr (ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF)
                return m_EntityToComponent.GetAliveContainer().end();
            else if constexpr (ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF == false)
                return CCSIteratorNoRefNoMap(nullptr);
        }
        else if constexpr (ComponentStorageInfo<ComponentType>::USE_MAP_VERSION)
            return CCSIteratorNoRefMap(m_EntityToComponent.GetContainer().end());
    }
    
    template <typename ComponentType>
    decltype(auto) CompressedComponentStorage<ComponentType>::cbegin() const
    {
        if constexpr (ComponentStorageInfo<ComponentType>::USE_MAP_VERSION == false)
        {
            if constexpr (ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF)
                return m_EntityToComponent.GetAliveContainer().cbegin();
            else if constexpr (ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF == false)
                return CCSIteratorNoRefNoMap(*this);
        }
        else if constexpr (ComponentStorageInfo<ComponentType>::USE_MAP_VERSION)
            return CCSIteratorNoRefMap(m_EntityToComponent.GetContainer().cbegin());
    }
    
    template <typename ComponentType>
    decltype(auto) CompressedComponentStorage<ComponentType>::cend() const
    {
       if constexpr (ComponentStorageInfo<ComponentType>::USE_MAP_VERSION == false)
        {
            if constexpr (ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF)
                return m_EntityToComponent.GetAliveContainer().cend();
            else if constexpr (ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF == false)
                return CCSIteratorNoRefNoMap(nullptr);
        }
        else if constexpr (ComponentStorageInfo<ComponentType>::USE_MAP_VERSION)
            return CCSIteratorNoRefMap(m_EntityToComponent.GetContainer().cend());
    }
}