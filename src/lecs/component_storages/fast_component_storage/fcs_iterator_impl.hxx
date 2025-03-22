#pragma once

#include "fast_component_storage.h"

#include "lecs/detail/iterable.h"

namespace lecs::detail {
    template <typename FastComponentStorage, typename ContainerIterator>
    class FCSIteratorNoRef {
    public:
        using iterator_category = std::forward_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = EntityId;
        using pointer           = const EntityId*;
        using reference         = EntityId;

    public:
        FCSIteratorNoRef(const FastComponentStorage* fastComponentStorage, ContainerIterator containerIterator, ContainerIterator containerIteratorLast)
            : fast_component_storage_(fastComponentStorage), container_iterator_(containerIterator), container_iterator_last_(containerIteratorLast) {}

    public:
        reference operator*() const {
            return container_iterator_.operator*();
        }
        pointer operator->() {
            return container_iterator_.operator->();
        }

        FCSIteratorNoRef& operator++() {
            bool currentEntityValid = false;
            do {
                ++container_iterator_;
                if (container_iterator_ == container_iterator_last_) break;

                EntityId entity    = *container_iterator_;
                currentEntityValid = fast_component_storage_->has_this_component(entity);
            } while (currentEntityValid == false);

            if (currentEntityValid == false) {
                fast_component_storage_ = nullptr;
            }

            return *this;
        }

        FCSIteratorNoRef operator++(int) {
            FCSIteratorNoRef res = this;
            ++(*this);
            return res;
        }

        bool operator==(const FCSIteratorNoRef& rhs) const {
#ifdef LECS_DEBUG
            bool complex_result = fast_component_storage_ != nullptr && fast_component_storage_ == rhs.fast_component_storage_ && container_iterator_ == rhs.container_iterator_ &&
                                  container_iterator_last_ == rhs.container_iterator_last_;
            bool simple_result = fast_component_storage_ != nullptr;

            LECS_ASSERT(simple_result == complex_result, "Operator== for Iterator is wrong")

            return simple_result;
#else
            return fast_component_storage_ != nullptr;
#endif
        }

        bool operator!=(const FCSIteratorNoRef& rhs) const {
            return !(*this == rhs);
        }

        bool operator==(IterableEnd rhs) {
            return fast_component_storage_ == nullptr;
        }
        bool operator!=(IterableEnd rhs) {
            return !(*this == rhs);
        }

    private:
        const FastComponentStorage* fast_component_storage_;
        ContainerIterator           container_iterator_;
        ContainerIterator           container_iterator_last_;
    };

    template <typename ComponentType>
        requires(TypeValidForComponentStorage<ComponentType>::value)
    decltype(auto) FastComponentStorage<ComponentType>::entities_iterator_begin() const
        requires(ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF && ComponentStorageInfo<ComponentType>::SEND_ENTITIES_POOL_ON_EACH == false)
    {
        return alive_entities_container_.cbegin();
    }
    template <typename ComponentType>
        requires(TypeValidForComponentStorage<ComponentType>::value)
    decltype(auto) FastComponentStorage<ComponentType>::entities_iterator_end() const
        requires(ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF && ComponentStorageInfo<ComponentType>::SEND_ENTITIES_POOL_ON_EACH == false)
    {
        return alive_entities_container_.cend();
    }

    template <typename ComponentType>
        requires(TypeValidForComponentStorage<ComponentType>::value)
    decltype(auto) FastComponentStorage<ComponentType>::entities_iterator_begin(const auto& alive_entities_registry) const
        requires(ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF == false && ComponentStorageInfo<ComponentType>::SEND_ENTITIES_POOL_ON_EACH)
    {
        return FCSIteratorNoRef(this, alive_entities_registry.cbegin(), alive_entities_registry.cend());
    }

    template <typename ComponentType>
        requires(TypeValidForComponentStorage<ComponentType>::value)
    decltype(auto) FastComponentStorage<ComponentType>::entities_iterator_end(const auto&) const
        requires(ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF == false && ComponentStorageInfo<ComponentType>::SEND_ENTITIES_POOL_ON_EACH)
    {
        return IterableEnd();
    }
}  // namespace lecs::detail
