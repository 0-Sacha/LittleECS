#pragma once

#include "basic_view.h"

#include "lecs/detail/iterable.h"

namespace lecs::detail
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
            : basic_view_linked_(basicViewLinked)
            , sub_entities_iterator_(subEntitiesIterator)
            , sub_entities_iterator_last_(subEntitiesIteratorLast)
        {}

    public:
        const BasicViewLinked* GetViewLinked() const
        {
            return basic_view_linked_;
        }

    public:
        reference operator*()
        {
            return *sub_entities_iterator_;
        }
        pointer operator->()
        {
            return sub_entities_iterator_.operator->();
        }

        ViewEntitiesIterator& operator++()
        {
            bool currentEntityValid = false;
            do
            {
                ++sub_entities_iterator_;
                if (sub_entities_iterator_ == sub_entities_iterator_last_)
                    break;

                if constexpr (sizeof...(IteratorComponentTypes) > 0)
                    currentEntityValid = basic_view_linked_->template has_all<IteratorComponentTypes...>(operator*());      
                else
                    currentEntityValid = true;
            } while(currentEntityValid == false);

            if (currentEntityValid == false)
            {
                basic_view_linked_ = nullptr;
            }

            return *this;
        }

        ViewEntitiesIterator operator++(int) { ViewEntitiesIterator res(basic_view_linked_, sub_entities_iterator_, sub_entities_iterator_last_); ++(*this); return res; }

        bool operator==(const ViewEntitiesIterator& rhs) const
        {
        #ifdef LECS_DEBUG
            bool complex_result = basic_view_linked_ != nullptr &&
                basic_view_linked_ == rhs.basic_view_linked_ &&
                sub_entities_iterator_ == rhs.sub_entities_iterator_ &&
                sub_entities_iterator_last_ == rhs.sub_entities_iterator_last_;
            bool simple_result = basic_view_linked_ != nullptr;

            LECS_ASSERT(simple_result == complex_result, "Operator== for Iterator is wrong")

            return simple_result;
        #else
            return basic_view_linked_ != nullptr;
        #endif
        }

        bool operator!=(const ViewEntitiesIterator& rhs) const { return !(*this == rhs); }

        bool operator==(IterableEnd rhs) { return basic_view_linked_ == nullptr; }
        bool operator!=(IterableEnd rhs) { return !(*this == rhs); }

    private:
        const BasicViewLinked* basic_view_linked_;
        SubEntitiesIterator sub_entities_iterator_;
        SubEntitiesIteratorLast sub_entities_iterator_last_;
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
            : basic_view_linked_(basicViewLinked)
            , sub_view_entities_iterator_(subViewEntitiesIterator)
            , sub_view_entities_iterator_last_(subViewEntitiesIteratorLast)
        {}

    public:
        value_type operator*()
        {
            EntityId entity = *sub_view_entities_iterator_;
            if constexpr (INCLUDE_ENTITY == true)
                return std::tuple_cat(std::tuple<EntityId>(entity), basic_view_linked_->template get_all<IteratorComponentTypes...>(entity));
            else
                return std::tuple_cat(basic_view_linked_->template get_all<IteratorComponentTypes...>(entity));
        }

        ViewComponentsIterator& operator++()
        {
            ++sub_view_entities_iterator_;
            if (sub_view_entities_iterator_ == sub_view_entities_iterator_last_)
                basic_view_linked_ = nullptr;
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
                bool complex_result = basic_view_linked_ != nullptr &&
                    basic_view_linked_ == rhs.basic_view_linked_ &&
                    sub_view_entities_iterator_ == rhs.sub_view_entities_iterator_ &&
                    sub_view_entities_iterator_ == sub_view_entities_iterator_last_;
                bool simple_result = basic_view_linked_ != nullptr;

                LECS_ASSERT(simple_result == complex_result, "Operator== for Iterator is wrong")

                return simple_result;
            #else
                return basic_view_linked_ != nullptr;
            #endif
        }

        bool operator!=(const ViewComponentsIterator& rhs) { return !(*this == rhs); }

        bool operator==(IterableEnd rhs) { return basic_view_linked_ == nullptr; }
        bool operator!=(IterableEnd rhs) { return !(*this == rhs); }

    private:
        BasicViewLinked* basic_view_linked_;
        SubViewEntitiesIterator sub_view_entities_iterator_;
        SubViewEntitiesIteratorLast sub_view_entities_iterator_last_;
    };
}
