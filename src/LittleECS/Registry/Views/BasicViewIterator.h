#pragma once

#include "BasicView.h"

#include "LittleECS/Detail/Iterable.h"

namespace LECS::Detail
{
    template <typename BasicViewLinked, typename SubEntitiesIterator, typename SubEntitiesIteratorLast, typename... IteratorComponentTypes>
    class ViewEntitiesIterator
    {
    public:
        using iterator_category = std::forward_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = EntityId;
        using pointer           = const EntityId*;
        using reference         = EntityId;

    public:
        ViewEntitiesIterator(const BasicViewLinked* basicViewLinked, SubEntitiesIterator subEntitiesIterator, SubEntitiesIteratorLast subEntitiesIteratorLast)
            : m_BasicViewLinked(basicViewLinked)
            , m_SubEntitiesIterator(subEntitiesIterator)
            , m_SubEntitiesIteratorLast(subEntitiesIteratorLast)
        {}

    public:
        const BasicViewLinked* GetViewLinked() const
        {
            return m_BasicViewLinked;
        }

    public:
        reference operator*()
        {
            return *m_SubEntitiesIterator;
        }
        pointer operator->()
        {
            return m_SubEntitiesIterator.operator->();
        }

        ViewEntitiesIterator& operator++()
        {
            bool currentEntityValid = false;
            do
            {
                ++m_SubEntitiesIterator;
                if (m_SubEntitiesIterator == m_SubEntitiesIteratorLast)
                    break;

                if constexpr (sizeof...(IteratorComponentTypes) > 0)
                    currentEntityValid = m_BasicViewLinked->template HasAll<IteratorComponentTypes...>(operator*());      
                else
                    currentEntityValid = true;
            } while(currentEntityValid == false);

            if (currentEntityValid == false)
            {
                m_BasicViewLinked = nullptr;
            }

            return *this;
        }

        ViewEntitiesIterator operator++(int) { ViewEntitiesIterator res(m_BasicViewLinked, m_SubEntitiesIterator, m_SubEntitiesIteratorLast); ++(*this); return res; }

        bool operator==(const ViewEntitiesIterator& rhs) const
        {
        #ifdef LECS_DEBUG
            bool complexResult = m_BasicViewLinked != nullptr &&
                m_BasicViewLinked == rhs.m_BasicViewLinked &&
                m_SubEntitiesIterator == rhs.m_SubEntitiesIterator &&
                m_SubEntitiesIteratorLast == rhs.m_SubEntitiesIteratorLast;
            bool simpleResult = m_BasicViewLinked != nullptr;

            LECS_ASSERT(simpleResult == complexResult, "Operator== for Iterator is wrong")

            return simpleResult;
        #else
            return m_BasicViewLinked != nullptr;
        #endif
        }

        bool operator!=(const ViewEntitiesIterator& rhs) const { return !(*this == rhs); }

        bool operator==(IterableEnd rhs) { return m_BasicViewLinked == nullptr; }
        bool operator!=(IterableEnd rhs) { return !(*this == rhs); }

    private:
        const BasicViewLinked* m_BasicViewLinked;
        SubEntitiesIterator m_SubEntitiesIterator;
        SubEntitiesIteratorLast m_SubEntitiesIteratorLast;
    };

    template <typename BasicViewLinked, typename SubViewEntitiesIterator, typename SubViewEntitiesIteratorLast, bool INCLUDE_ENTITY, typename... IteratorComponentTypes>
    class ViewComponentsIterator
    {
    public:
        using iterator_category = std::forward_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        
        using value_type        = std::conditional_t<INCLUDE_ENTITY, 
                                    std::tuple<EntityId, IteratorComponentTypes&...>,
                                    std::tuple<IteratorComponentTypes&...>>;

        using pointer           = value_type;
        using reference         = value_type;

    public:
        ViewComponentsIterator(BasicViewLinked* basicViewLinked, SubViewEntitiesIterator subViewEntitiesIterator, SubViewEntitiesIteratorLast subViewEntitiesIteratorLast)
            : m_BasicViewLinked(basicViewLinked)
            , m_SubViewEntitiesIterator(subViewEntitiesIterator)
            , m_SubViewEntitiesIteratorLast(subViewEntitiesIteratorLast)
        {}

    public:
        value_type operator*()
        {
            EntityId entity = *m_SubViewEntitiesIterator;
            if constexpr (INCLUDE_ENTITY == true)
                return std::tuple_cat(std::tuple<EntityId>(entity), m_BasicViewLinked->template GetAll<IteratorComponentTypes...>(entity));
            else
                return std::tuple_cat(m_BasicViewLinked->template GetAll<IteratorComponentTypes...>(entity));
        }

        ViewComponentsIterator& operator++()
        {
            ++m_SubViewEntitiesIterator;
            if (m_SubViewEntitiesIterator == m_SubViewEntitiesIteratorLast)
                m_BasicViewLinked = nullptr;
            return *this;
        }

        ViewComponentsIterator operator++(int)
        {
            ViewComponentsIterator res = *this;
            ++(*this);
            return res;            
        }

        bool operator==(const ViewComponentsIterator& rhs)
        {
            #ifdef LECS_DEBUG
                bool complexResult = m_BasicViewLinked != nullptr &&
                    m_BasicViewLinked == rhs.m_BasicViewLinked &&
                    m_SubViewEntitiesIterator == rhs.m_SubViewEntitiesIterator &&
                    m_SubViewEntitiesIterator == m_SubViewEntitiesIteratorLast;
                bool simpleResult = m_BasicViewLinked != nullptr;

                LECS_ASSERT(simpleResult == complexResult, "Operator== for Iterator is wrong")

                return simpleResult;
            #else
                return m_BasicViewLinked != nullptr;
            #endif
        }

        bool operator!=(const ViewComponentsIterator& rhs) { return !(*this == rhs); }

        bool operator==(IterableEnd rhs) { return m_BasicViewLinked == nullptr; }
        bool operator!=(IterableEnd rhs) { return !(*this == rhs); }

    private:
        BasicViewLinked* m_BasicViewLinked;
        SubViewEntitiesIterator m_SubViewEntitiesIterator;
        SubViewEntitiesIteratorLast m_SubViewEntitiesIteratorLast;
    };
}
