#pragma once

#include "../IComponentStorage.h"

#include <array>

namespace LittleECS::Detail
{
    template <typename ComponentType, std::size_t PAGE_SIZE>
    requires (PAGE_SIZE % sizeof(std::size_t) == 0)
    class CompressedComponentStoragePage
    {
    private:
        struct ComponentDataBuffer
        {
            union DataStorageType
            {
                std::uint8_t StorageData[sizeof(ComponentType)];
                ComponentType ComponentValue;
            };
            DataStorageType Data{};
        };

        static constexpr std::size_t NUMBER_OF_BLOCKS = PAGE_SIZE / sizeof(std::size_t);
        static constexpr std::size_t BLOCK_SIZE = sizeof(std::size_t) * 8;

	private:
        template <typename... Args>
		inline ComponentType& ConstructAt(Index::PageIndexOfComponent index, Args&&... args)
		{
			ComponentDataBuffer* buffer = &m_Page[index];
			ComponentType* component = new (buffer) ComponentType(std::forward<Args>(args)...);
            return *component;
		}

        inline void DestroyAt(Index::PageIndexOfComponent index)
        {
            ComponentType& component = m_Page[index].Data.ComponentValue;
            component.~ComponentType();
        }

    public:
        CompressedComponentStoragePage()
        {
            for (std::size_t i = 0; i < NUMBER_OF_BLOCKS; ++i)
                m_FreeComponent[i] = std::numeric_limits<std::size_t>::max();

#ifdef LECS_DEBUG
            for (std::size_t i = 0; i < PAGE_SIZE; ++i)
                m_EntityIdLinked[i] = EntityId::INVALID;
#endif
        }

		~CompressedComponentStoragePage() = default;

    protected:
        std::array<ComponentDataBuffer, PAGE_SIZE> m_Page;
		std::size_t m_FreeComponent[NUMBER_OF_BLOCKS];
		EntityId m_EntityIdLinked[PAGE_SIZE];
        std::size_t m_CurrentSize = 0;

    public:
        inline bool HasComponentAtIndex(Index::PageIndexOfComponent index) const
        {
            std::size_t indexOfBlock = index / (BLOCK_SIZE);
            std::size_t block = *(m_FreeComponent + indexOfBlock);
            std::size_t indexInBlock = index % (BLOCK_SIZE);
            return (block & (static_cast<std::size_t>(1) << indexInBlock)) == 0;
        }

    private:
        inline void SetHasComponentAtIndex(Index::PageIndexOfComponent index, bool has)
        {
            std::size_t indexOfBlock = index / (BLOCK_SIZE);
            std::size_t* block = m_FreeComponent + indexOfBlock;
            std::size_t indexInBlock = index % (BLOCK_SIZE);

            *block = *block & ~(static_cast<std::size_t>(1) << indexInBlock);

            if (has == false)
                *block |= (static_cast<std::size_t>(1) << indexInBlock);
        }

        Index::PageIndexOfComponent GetNextFreeIndex() const
        {
            const std::size_t* beginFreeListBlocks = m_FreeComponent;
            const std::size_t* endFreeListBlocks = m_FreeComponent + NUMBER_OF_BLOCKS;
            while (beginFreeListBlocks < endFreeListBlocks)
            {
                if (*beginFreeListBlocks != 0)
                    break;
                ++beginFreeListBlocks;
            }

			LECS_ASSERT(beginFreeListBlocks != endFreeListBlocks, "This page is full")
			LECS_ASSERT(*beginFreeListBlocks != 0, "This page is full")

			std::size_t blockIndex = static_cast<std::size_t>(beginFreeListBlocks - m_FreeComponent);

            std::size_t block = *beginFreeListBlocks;
            std::size_t mask = 1;
            std::uint8_t freeIndexInBlock = 0;
            for(; freeIndexInBlock < BLOCK_SIZE; ++freeIndexInBlock)
            {
                if (block & mask)
                    break;
                mask = mask << 1;
            }

            LECS_ASSERT(freeIndexInBlock != (BLOCK_SIZE), "The block found is full")

            std::size_t foundIndex = freeIndexInBlock + (blockIndex * BLOCK_SIZE);

            return foundIndex;
        }

        Index::PageIndexOfComponent GetNextValidIndex(Index::PageIndexOfComponent index) const
        {
            std::size_t blockIndex = index / sizeof(std::size_t);
            std::size_t subBlockIndex = index % sizeof(std::size_t);

            LECS_ASSERT(blockIndex < NUMBER_OF_BLOCKS, "This index can't exist")
            LECS_ASSERT(subBlockIndex < BLOCK_SIZE, "This sub-index can't exist")

            const std::size_t* currentBlock = m_FreeComponent + index;
            const std::size_t* endBlock = m_FreeComponent + NUMBER_OF_BLOCKS;
            std::uint8_t indexInSubBlock = subBlockIndex + 1;

            std::size_t foundIndex = PAGE_SIZE;

            while (currentBlock < endBlock)
            {
                std::size_t mask = static_cast<std::size_t>(1) << indexInSubBlock;
                for(; indexInSubBlock < BLOCK_SIZE; ++indexInSubBlock)
                {
                    if ((*currentBlock & mask) == 0)
                    {
                        foundIndex = indexInSubBlock + ((endBlock - currentBlock) * BLOCK_SIZE);
                        currentBlock = endBlock;
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
        inline bool CanAddComponent() const { return m_CurrentSize + 1 < PAGE_SIZE; }

        template<typename... Args>
        std::pair<Index::PageIndexOfComponent, ComponentType&> AddComponent(EntityId entity, Args&&... args)
        {
            LECS_ASSERT(CanAddComponent(), "Can't add more component to this page")

            Index::PageIndexOfComponent index = GetNextFreeIndex();
			LECS_ASSERT(HasComponentAtIndex(index) == false, "There are already a component at this index")

            ComponentType& component = ConstructAt(index, std::forward<Args>(args)...);
            SetHasComponentAtIndex(index, true);
            m_EntityIdLinked[index] = entity;
            ++m_CurrentSize;
            return { index, component };
        }

        void RemoveComponentAtIndex(Index::PageIndexOfComponent index)
        {
            LECS_ASSERT(HasComponentAtIndex(index) == true, "There are no component at this index")

            DestroyAt(index);
            SetHasComponentAtIndex(index, false);
            m_EntityIdLinked[index] = EntityId::INVALID;
            --m_CurrentSize;
        }

        ComponentType& GetComponentAtIndex(Index::PageIndexOfComponent index)
        {
            LECS_ASSERT(HasComponentAtIndex(index) == true, "There are no component at this index")
			LECS_ASSERT(m_EntityIdLinked[index] != EntityId::INVALID, "Not supposed to have a valid component linked to a non valid entityId")

            return *reinterpret_cast<ComponentType*>(&m_Page[index]);
		}

        const ComponentType& GetComponentAtIndex(Index::PageIndexOfComponent index) const
        {
            LECS_ASSERT(HasComponentAtIndex(index) == true, "There are no component at this index")
			LECS_ASSERT(m_EntityIdLinked[index] != EntityId::INVALID, "Not supposed to have a valid component linked to a non valid entityId")

            return *reinterpret_cast<const ComponentType*>(&m_Page[index]);
		}

        ComponentType* GetComponentAtIndexPtr(Index::PageIndexOfComponent index)
        {
            if (HasComponentAtIndex(index) == false)
                return nullptr;
            if (m_EntityIdLinked[index] != EntityId::INVALID)
                return nullptr;
            return reinterpret_cast<ComponentType*>(&m_Page[index]);
		}

        const ComponentType* GetComponentAtIndexPtr(Index::PageIndexOfComponent index) const
        {
            if (HasComponentAtIndex(index) == false)
                return nullptr;
            if (m_EntityIdLinked[index] != EntityId::INVALID)
                return nullptr;
            return reinterpret_cast<const ComponentType*>(&m_Page[index]);
		}

        inline EntityId GetEntityIdAtIndex(Index::PageIndexOfComponent index) const
		{
			return m_EntityIdLinked[index];
        }

    private:
        template <typename Function, typename ComponentConstness>
		void ForEachPageImpl(Function&& function);

    public:
        template <typename Function>
		inline void ForEachPage(Function&& function)
        {
            return ForEachPageImpl<Function, ComponentType>(function);
        }

        template <typename Function>
		void ForEachPage(Function&& function) const
        {
            return const_cast<CompressedComponentStoragePage<ComponentType, PAGE_SIZE>*>(this)->template ForEachPageImpl<Function, const ComponentType>(function);
        }
    };
}
 
#include "CCSPageEach.h"
