#pragma once

#include "CompressedComponentStoragePage.h"

namespace LECS::Detail
{
    template <typename ComponentType, std::size_t PAGE_SIZE>
    requires (PAGE_SIZE % sizeof(std::size_t) == 0)
    template <typename Function, typename ComponentConstness> // Function = std::function<void(Index::PageIndexOfComponent)>
    void CompressedComponentStoragePage<ComponentType, PAGE_SIZE>::ForEachPageImpl(Function&& function)
    {
        const std::size_t* beginFreeListBlocks = m_FreeComponent;
        const std::size_t* endFreeListBlocks = m_FreeComponent + NUMBER_OF_BLOCKS;
        while (beginFreeListBlocks < endFreeListBlocks)
        {
            if (*beginFreeListBlocks != std::numeric_limits<std::size_t>::max())
            {
                std::size_t blockIndex = beginFreeListBlocks - m_FreeComponent;
                Index::PageIndexOfComponent blockShift = (blockIndex * BLOCK_SIZE);

                std::size_t block = *beginFreeListBlocks;
                std::size_t mask = 1;
                std::uint8_t freeIndexInBlock = 0;
                for (; freeIndexInBlock < BLOCK_SIZE; ++freeIndexInBlock)
                {
                    if ((block & mask) == 0)
                    {
                        Index::PageIndexOfComponent index = freeIndexInBlock + blockShift;
                        
                        function(index);
                    }
                    mask = mask << 1;
                }

            }
            ++beginFreeListBlocks;
        }
    }
}
