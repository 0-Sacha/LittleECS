#pragma once

#include "CompressedComponentStorage.h"

#include "LittleECS/Detail/Iterable.h"

namespace LittleECS::Detail
{
    namespace CustomIterator
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
            CCSIteratorNoRefNoMap(const CompressedComponentStorage* compressedComponentStorage, std::size_t currentIndexOfPage = 0, std::size_t currentPageIndexOfCurrent = 0)
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
            
                return m_CompressedComponentStorage->GetPageContainer()[m_CurrentIndexOfPage].HasComponentAtIndex(m_CurrentPageIndexOfCurrent);
            }

            EntityId* GetCurrentEntity()
            {
                LECS_ASSERT(CurrentEntityIsValid(), "Invalid Iterator")
                return &m_CompressedComponentStorage->GetPageContainer()[m_CurrentIndexOfPage].GetEntityIdAtIndex(m_CurrentPageIndexOfCurrent);
            }

        public:
            reference operator*()
            {
                return *GetCurrentEntity();
            }
            pointer operator->()
            {
                return GetCurrentEntity();
            }

            CCSIteratorNoRefNoMap& operator++()
            {
                bool found = false;
                while (m_CurrentIndexOfPage < m_CompressedComponentStorage->GetPageContainer().size())
                {
                    m_CurrentPageIndexOfCurrent = m_CompressedComponentStorage->GetPageContainer()[m_CurrentIndexOfPage].GetNextValidIndex(m_CurrentPageIndexOfCurrent);
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

            bool operator==(const CCSIteratorNoRefNoMap& rhs) const
            {
            #ifdef LECS_DEBUG
                bool complexResult = m_CompressedComponentStorage != nullptr &&
                    m_CompressedComponentStorage == rhs.m_CompressedComponentStorage &&
                    m_CurrentIndexOfPage == rhs.m_CurrentIndexOfPage &&
                    m_CurrentPageIndexOfCurrent == rhs.m_CurrentPageIndexOfCurrent;
                bool simpleResult = m_CompressedComponentStorage != nullptr;

                LECS_ASSERT(simpleResult == complexResult, "Operator== for Iterator is wrong")

                return simpleResult;
            #else
                return m_CompressedComponentStorage != nullptr;
            #endif
            }

            bool operator!=(const CCSIteratorNoRefNoMap& rhs) const { return !(*this == rhs); }

            bool operator==(IterableEnd rhs) { return m_CompressedComponentStorage == nullptr; }
            bool operator!=(IterableEnd rhs) { return !(*this == rhs); }

        private:
            const CompressedComponentStorage* m_CompressedComponentStorage;
            std::size_t m_CurrentIndexOfPage;
            std::size_t m_CurrentPageIndexOfCurrent;
        };

        template <typename SubIterator>
        class CCSIteratorNoRefMap
        {
        public:
            using iterator_category = typename SubIterator::iterator_category;
            using difference_type   = typename SubIterator::difference_type;
            using value_type        = EntityId;
            using pointer           = const EntityId*;
            using reference         = EntityId;

        public:
            CCSIteratorNoRefMap(SubIterator mapRefIterator)
                : m_MapRefIterator(mapRefIterator)
            {}

        public:
            reference operator*()
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

            CCSIteratorNoRefMap& operator++(int)
            {
                return m_MapRefIterator++;
            }

            bool operator==(const CCSIteratorNoRefMap& rhs) const
            {
                return m_MapRefIterator == rhs.m_MapRefIterator;
            }

            bool operator!=(const CCSIteratorNoRefMap& rhs) const { return !(*this == rhs); }

        private:
            SubIterator m_MapRefIterator;
        };
    }

    template <typename ComponentType>
    decltype(auto) CompressedComponentStorage<ComponentType>::cbegin() const
    {
        if constexpr (ComponentStorageInfo<ComponentType>::USE_MAP_VERSION == false)
        {
            if constexpr (ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF)
                return m_EntityToComponent.GetAliveContainer().cbegin();
            else if constexpr (ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF == false)
                return CustomIterator::CCSIteratorNoRefNoMap<M_Type>(*this);
        }
        else if constexpr (ComponentStorageInfo<ComponentType>::USE_MAP_VERSION)
            return CustomIterator::CCSIteratorNoRefMap(m_EntityToComponent.GetContainer().cbegin());
    }
    
    template <typename ComponentType>
    decltype(auto) CompressedComponentStorage<ComponentType>::cend() const
    {
       if constexpr (ComponentStorageInfo<ComponentType>::USE_MAP_VERSION == false)
        {
            if constexpr (ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF)
                return m_EntityToComponent.GetAliveContainer().cend();
            else if constexpr (ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF == false)
                return IterableEnd();
        }
        else if constexpr (ComponentStorageInfo<ComponentType>::USE_MAP_VERSION)
            return CustomIterator::CCSIteratorNoRefMap(m_EntityToComponent.GetContainer().cend());
    }
}
