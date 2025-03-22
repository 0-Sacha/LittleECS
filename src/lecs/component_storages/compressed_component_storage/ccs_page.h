#pragma once

#include "lecs/registry/component_storage.h"

#include <array>

namespace lecs::detail {
    template <typename ComponentType, std::size_t PAGE_SIZE>
        requires(PAGE_SIZE % sizeof(std::size_t) == 0)
    class CompressedComponentStoragePage {
    private:
        struct ComponentDataBuffer {
            union DataStorageType {
                std::uint8_t  storage_data[sizeof(ComponentType)];
                ComponentType component_value;

                DataStorageType() {}
                ~DataStorageType() {}
            };
            DataStorageType data{};
        };

        static constexpr std::size_t NUMBER_OF_BLOCKS = PAGE_SIZE / sizeof(std::size_t);
        static constexpr std::size_t BLOCK_SIZE       = sizeof(std::size_t) * 8;

    private:
        template <typename... Args>
        inline ComponentType& construct_at(Index::ComponentPageIndex index, Args&&... args) {
            ComponentDataBuffer* buffer    = &page_[index];
            ComponentType*       component = new (buffer) ComponentType(std::forward<Args>(args)...);
            return *component;
        }

        inline void destroy_at(Index::ComponentPageIndex index) {
            ComponentType& component = page_[index].data.component_value;
            component.~ComponentType();
        }

    public:
        CompressedComponentStoragePage() {
            for (std::size_t i = 0; i < NUMBER_OF_BLOCKS; ++i)
                free_component_[i] = std::numeric_limits<std::size_t>::max();

#ifdef LECS_DEBUG
            for (std::size_t i = 0; i < PAGE_SIZE; ++i)
                entityid_linked_[i] = EntityId::INVALID;
#endif
        }

        ~CompressedComponentStoragePage() = default;

    protected:
        std::array<ComponentDataBuffer, PAGE_SIZE> page_;
        std::size_t                                free_component_[NUMBER_OF_BLOCKS];
        EntityId                                   entityid_linked_[PAGE_SIZE];
        std::size_t                                current_size_ = 0;

    public:
        inline bool has_component_at_index(Index::ComponentPageIndex index) const {
            std::size_t indexOfBlock = index / (BLOCK_SIZE);
            std::size_t block        = *(free_component_ + indexOfBlock);
            std::size_t indexInBlock = index % (BLOCK_SIZE);
            return (block & (static_cast<std::size_t>(1) << indexInBlock)) == 0;
        }

    private:
        inline void set_has_component_at_index(Index::ComponentPageIndex index, bool has) {
            std::size_t  indexOfBlock = index / (BLOCK_SIZE);
            std::size_t* block        = free_component_ + indexOfBlock;
            std::size_t  indexInBlock = index % (BLOCK_SIZE);

            *block = *block & ~(static_cast<std::size_t>(1) << indexInBlock);

            if (has == false) *block |= (static_cast<std::size_t>(1) << indexInBlock);
        }

        Index::ComponentPageIndex GetNextFreeIndex() const {
            const std::size_t* begin_free_list_blocks = free_component_;
            const std::size_t* end_free_list_blocks   = free_component_ + NUMBER_OF_BLOCKS;
            while (begin_free_list_blocks < end_free_list_blocks) {
                if (*begin_free_list_blocks != 0) break;
                ++begin_free_list_blocks;
            }

            LECS_ASSERT(begin_free_list_blocks != end_free_list_blocks, "This page is full")
            LECS_ASSERT(*begin_free_list_blocks != 0, "This page is full")

            std::size_t blockIndex = static_cast<std::size_t>(begin_free_list_blocks - free_component_);

            std::size_t  block               = *begin_free_list_blocks;
            std::size_t  mask                = 1;
            std::uint8_t free_index_in_block = 0;
            for (; free_index_in_block < BLOCK_SIZE; ++free_index_in_block) {
                if (block & mask) break;
                mask = mask << 1;
            }

            LECS_ASSERT(free_index_in_block != (BLOCK_SIZE), "The block found is full")

            std::size_t foundIndex = free_index_in_block + (blockIndex * BLOCK_SIZE);

            return foundIndex;
        }

    public:
        Index::ComponentPageIndex get_next_valid_index(Index::ComponentPageIndex index) const {
            std::size_t blockIndex    = index / sizeof(std::size_t);
            std::size_t subBlockIndex = index % sizeof(std::size_t);

            LECS_ASSERT(blockIndex < NUMBER_OF_BLOCKS, "This index can't exist")
            LECS_ASSERT(subBlockIndex < BLOCK_SIZE, "This sub-index can't exist")

            const std::size_t* currentBlock    = free_component_ + index;
            const std::size_t* endBlock        = free_component_ + NUMBER_OF_BLOCKS;
            std::uint8_t       indexInSubBlock = subBlockIndex + 1;

            std::size_t foundIndex = PAGE_SIZE;

            while (currentBlock < endBlock) {
                std::size_t mask = static_cast<std::size_t>(1) << indexInSubBlock;
                for (; indexInSubBlock < BLOCK_SIZE; ++indexInSubBlock) {
                    if ((*currentBlock & mask) == 0) {
                        std::size_t blockIndex = NUMBER_OF_BLOCKS - std::size_t(endBlock - currentBlock);
                        foundIndex             = indexInSubBlock + (blockIndex * BLOCK_SIZE);
                        currentBlock           = endBlock;
                        break;
                    }
                    mask = mask << 1;
                }
                ++currentBlock;
                indexInSubBlock = 0;
            }

            return foundIndex;
        }

    public:
        inline bool can_add_component() const {
            return current_size_ + 1 < PAGE_SIZE;
        }

        template <typename... Args>
        std::pair<Index::ComponentPageIndex, ComponentType&> add_component(EntityId entity, Args&&... args) {
            LECS_ASSERT(can_add_component(), "Can't add more component to this page")

            Index::ComponentPageIndex index = GetNextFreeIndex();
            LECS_ASSERT(has_component_at_index(index) == false, "There are already a component at this index")

            ComponentType& component = construct_at(index, std::forward<Args>(args)...);
            set_has_component_at_index(index, true);
            entityid_linked_[index] = entity;
            ++current_size_;
            return {index, component};
        }

        void remove_component_at_index(Index::ComponentPageIndex index) {
            LECS_ASSERT(has_component_at_index(index) == true, "There are no component at this index")

            destroy_at(index);
            set_has_component_at_index(index, false);
            entityid_linked_[index] = EntityId::INVALID;
            --current_size_;
        }

        ComponentType& get_component_at_index(Index::ComponentPageIndex index) {
            LECS_ASSERT(has_component_at_index(index) == true, "There are no component at this index")
            LECS_ASSERT(entityid_linked_[index] != EntityId::INVALID, "Not supposed to have a valid component linked to a non valid entityid")

            return *reinterpret_cast<ComponentType*>(&page_[index]);
        }

        const ComponentType& get_component_at_index(Index::ComponentPageIndex index) const {
            LECS_ASSERT(has_component_at_index(index) == true, "There are no component at this index")
            LECS_ASSERT(entityid_linked_[index] != EntityId::INVALID, "Not supposed to have a valid component linked to a non valid entityid")

            return *reinterpret_cast<const ComponentType*>(&page_[index]);
        }

        ComponentType* get_component_at_indexptr(Index::ComponentPageIndex index) {
            if (has_component_at_index(index) == false) return nullptr;
            if (entityid_linked_[index] != EntityId::INVALID) return nullptr;
            return reinterpret_cast<ComponentType*>(&page_[index]);
        }

        const ComponentType* get_component_at_indexptr(Index::ComponentPageIndex index) const {
            if (has_component_at_index(index) == false) return nullptr;
            if (entityid_linked_[index] != EntityId::INVALID) return nullptr;
            return reinterpret_cast<const ComponentType*>(&page_[index]);
        }

        inline EntityId get_entityid_at_index(Index::ComponentPageIndex index) const {
            return entityid_linked_[index];
        }

    private:
        template <typename Function, typename ComponentConstness>
        void foreach_pageImpl(Function&& function);

    public:
        template <typename Function>
        inline void foreach_page(Function&& function) {
            return foreach_pageImpl<Function, ComponentType>(std::forward<Function>(function));
        }

        template <typename Function>
        void foreach_page(Function&& function) const {
            return const_cast<CompressedComponentStoragePage<ComponentType, PAGE_SIZE>*>(this)->template foreach_pageImpl<Function, const ComponentType>(
                std::forward<Function>(function));
        }
    };
}  // namespace lecs::detail

#include "ccs_page_foreach-inl.h"
