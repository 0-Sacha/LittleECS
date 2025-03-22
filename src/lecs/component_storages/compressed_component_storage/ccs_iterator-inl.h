#pragma once

#include "compressed_component_storage.h"

#include "lecs/detail/iterable.h"

namespace lecs::detail
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
                : compressed_component_storage_(compressedComponentStorage)
                , current_index_of_page_(currentIndexOfPage)
                , current_page_index_(currentPageIndexOfCurrent)
            {
                if (current_entity_is_valid() == false)
                {
                    if (currentIndexOfPage == 0 && currentPageIndexOfCurrent == 0)
                        operator++();
                    else
                        compressed_component_storage_ = nullptr;
                }
            }

        private:
            bool current_entity_is_valid() const
            {
                if (compressed_component_storage_ == nullptr)
                    return false;

                if (current_index_of_page_ >= compressed_component_storage_->get_page_container().size())
                    return false;

                if (current_page_index_ >= CompressedComponentStorage::PAGE_SIZE)
                    return false;
            
                return compressed_component_storage_->get_page_container()[current_index_of_page_]->has_component_at_index(current_page_index_);
            }


        public:
            EntityId operator*() const
            {
                LECS_ASSERT(current_entity_is_valid(), "Invalid Iterator")
                return compressed_component_storage_->get_page_container()[current_index_of_page_]->get_entityid_at_index(current_page_index_);
            }

            CCSIteratorNoRefNoMap& operator++()
            {
                bool found = false;
                while (current_index_of_page_ < compressed_component_storage_->get_page_container().size())
                {
                    current_page_index_ = compressed_component_storage_->get_page_container()[current_index_of_page_]->get_next_valid_index(current_page_index_);
                    if (current_page_index_ < CompressedComponentStorage::PAGE_SIZE)
                    {
                        found = true;
                        break;
                    }
                    ++current_index_of_page_;
                }

                if (found == false)
                {
                    compressed_component_storage_ = nullptr;
                }

                return *this;
            }

            CCSIteratorNoRefNoMap operator++(int) { CCSIteratorNoRefNoMap res(compressed_component_storage_, current_index_of_page_, current_page_index_); ++(*this); return res; }

            bool operator==(const CCSIteratorNoRefNoMap& rhs) const
            {
            #ifdef LECS_DEBUG
                bool complex_result = compressed_component_storage_ != nullptr &&
                    compressed_component_storage_ == rhs.compressed_component_storage_ &&
                    current_index_of_page_ == rhs.current_index_of_page_ &&
                    current_page_index_ == rhs.current_page_index_;
                bool simple_result = compressed_component_storage_ != nullptr;

                LECS_ASSERT(simple_result == complex_result, "Operator== for Iterator is wrong")

                return simple_result;
            #else
                return compressed_component_storage_ != nullptr;
            #endif
            }

            bool operator!=(const CCSIteratorNoRefNoMap& rhs) const { return !(*this == rhs); }

            bool operator==(IterableEnd rhs) { return compressed_component_storage_ == nullptr; }
            bool operator!=(IterableEnd rhs) { return !(*this == rhs); }

        private:
            const CompressedComponentStorage* compressed_component_storage_;
            std::size_t current_index_of_page_;
            std::size_t current_page_index_;
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
            CCSIteratorNoRefMap(SubIterator map_ref_iterator)
                : map_ref_iterator_(map_ref_iterator)
            {}

        public:
            reference operator*()
            {
                return map_ref_iterator_->second;
            }
            pointer operator->()
            {
                return &map_ref_iterator_->second;
            }

            CCSIteratorNoRefMap& operator++()
            {
                return ++map_ref_iterator_;
            }

            CCSIteratorNoRefMap& operator++(int)
            {
                return map_ref_iterator_++;
            }

            bool operator==(const CCSIteratorNoRefMap& rhs) const
            {
                return map_ref_iterator_ == rhs.map_ref_iterator_;
            }

            bool operator!=(const CCSIteratorNoRefMap& rhs) const { return !(*this == rhs); }

        private:
            SubIterator map_ref_iterator_;
        };
    }

    template <typename ComponentType>
    requires (TypeValidForComponentStorage<ComponentType>::value)
    decltype(auto) CompressedComponentStorage<ComponentType>::entities_iterator_begin() const
    {
        if constexpr (ComponentStorageInfo<ComponentType>::USE_MAP_VERSION == false)
        {
            if constexpr (ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF)
                return entity_to_component_.get_alive_container().cbegin();
            else if constexpr (ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF == false)
                return CustomIterator::CCSIteratorNoRefNoMap<M_Type>(this);
        }
        else if constexpr (ComponentStorageInfo<ComponentType>::USE_MAP_VERSION)
            return CustomIterator::CCSIteratorNoRefMap(entity_to_component_.get_container().entities_iterator_begin());
    }
    
    template <typename ComponentType>
    requires (TypeValidForComponentStorage<ComponentType>::value)
    decltype(auto) CompressedComponentStorage<ComponentType>::entities_iterator_end() const
    {
       if constexpr (ComponentStorageInfo<ComponentType>::USE_MAP_VERSION == false)
        {
            if constexpr (ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF)
                return entity_to_component_.get_alive_container().cend();
            else if constexpr (ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF == false)
                return IterableEnd();
        }
        else if constexpr (ComponentStorageInfo<ComponentType>::USE_MAP_VERSION)
            return CustomIterator::CCSIteratorNoRefMap(entity_to_component_.get_container().entities_iterator_end());
    }
}
