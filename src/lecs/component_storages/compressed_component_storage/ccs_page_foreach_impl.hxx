#pragma once

#include "ccs_page.h"

namespace lecs::detail {
    template <typename ComponentType, std::size_t PAGE_SIZE>
        requires(PAGE_SIZE % sizeof(std::size_t) == 0)
    template <typename Function, typename ComponentConstness>  // Function = std::function<void(Index::ComponentPageIndex)>
    void CompressedComponentStoragePage<ComponentType, PAGE_SIZE>::foreach_pageImpl(Function&& function) {
        const std::size_t* begin_free_list_blocks = free_component_;
        const std::size_t* end_free_list_blocks   = free_component_ + NUMBER_OF_BLOCKS;
        while (begin_free_list_blocks < end_free_list_blocks) {
            if (*begin_free_list_blocks != std::numeric_limits<std::size_t>::max()) {
                std::size_t               blockIndex  = begin_free_list_blocks - free_component_;
                Index::ComponentPageIndex block_shift = (blockIndex * BLOCK_SIZE);

                std::size_t  block               = *begin_free_list_blocks;
                std::size_t  mask                = 1;
                std::uint8_t free_index_in_block = 0;
                for (; free_index_in_block < BLOCK_SIZE; ++free_index_in_block) {
                    if ((block & mask) == 0) {
                        Index::ComponentPageIndex index = free_index_in_block + block_shift;

                        function(index);
                    }
                    mask = mask << 1;
                }
            }
            ++begin_free_list_blocks;
        }
    }
}  // namespace lecs::detail
