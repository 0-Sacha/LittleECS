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
		std::size_t m_FreeComponent[PAGE_SIZE / sizeof(std::size_t)];
		EntityId m_EntityIdLinked[PAGE_SIZE];
        std::size_t m_CurrentSize = 0;

    public:
        inline bool HasComponentAtIndex(Index::PageIndexOfComponent index) const
        {
            std::size_t indexOfBlock = index / (sizeof(std::size_t) * 8);
            std::size_t block = *(m_FreeComponent + indexOfBlock);
            std::size_t indexInBlock = index % (sizeof(std::size_t) * 8);
            return block & (static_cast<std::size_t>(1) << indexInBlock);
        }

    private:
        inline void SetHasComponentAtIndex(Index::PageIndexOfComponent index, bool set)
        {
            std::size_t indexOfBlock = index / (sizeof(std::size_t) * 8);
            std::size_t* block = m_FreeComponent + indexOfBlock;
            std::size_t indexInBlock = index % (sizeof(std::size_t) * 8);

            *block = *block & ~(static_cast<std::size_t>(1) << indexInBlock);

            if (set)
                *block |= (static_cast<std::size_t>(1) << indexInBlock);
        }

        Index::PageIndexOfComponent GetNextIndex() const
        {
            const std::size_t* beginFreeListBlocks = m_FreeComponent;
            const std::size_t* endFreeListBlocks = m_FreeComponent + NUMBER_OF_BLOCKS;
            while (beginFreeListBlocks < endFreeListBlocks)
            {
                if (*beginFreeListBlocks != 0)
                    break;
                ++beginFreeListBlocks;
            }

			LECS_ASSERT(beginFreeListBlocks != endFreeListBlocks, "This page is full");
			LECS_ASSERT(*beginFreeListBlocks != 0, "This page is full");

			std::size_t blockIndex = beginFreeListBlocks - m_FreeComponent;

            std::size_t block = *beginFreeListBlocks;
            std::size_t mask = 1;
            std::uint8_t freeIndexInBlock = 0;
            for(; freeIndexInBlock < sizeof(std::size_t) * 8; ++freeIndexInBlock)
            {
                if (block & mask)
                    break;
                mask = mask << 1;
            }

            LECS_ASSERT(freeIndexInBlock != (sizeof(std::size_t) * 8), "The block found is full");

            std::size_t foundIndex = freeIndexInBlock + (blockIndex * sizeof(std::size_t) * 8);

            return foundIndex;
        }

    public:
        template <typename Function>
		void ForEach(Function&& function) const
		{
			const std::size_t* beginFreeListBlocks = m_FreeComponent;
            const std::size_t* endFreeListBlocks = m_FreeComponent + NUMBER_OF_BLOCKS;
            while (beginFreeListBlocks < endFreeListBlocks)
            {
                if (*beginFreeListBlocks != std::numeric_limits<std::size_t>::max())
                {
                    std::size_t blockIndex = beginFreeListBlocks - m_FreeComponent;
                    Index::PageIndexOfComponent blockShift = (blockIndex * sizeof(std::size_t) * 8);

                    std::size_t block = *beginFreeListBlocks;
                    std::size_t mask = 1;
                    std::uint8_t freeIndexInBlock = 0;
                    for (; freeIndexInBlock < sizeof(std::size_t) * 8; ++freeIndexInBlock)
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

    public:
        inline bool CanAddComponent() const { return m_CurrentSize + 1 < PAGE_SIZE; }

        template<typename... Args>
        std::pair<Index::PageIndexOfComponent, ComponentType&> AddComponent(EntityId entity, Args&&... args)
        {
            LECS_ASSERT(CanAddComponent(), "Can't add more component to this page");

            Index::PageIndexOfComponent index = GetNextIndex();
            ComponentType& component = ConstructAt(index, std::forward<Args>(args)...);
            SetHasComponentAtIndex(index, false);
            m_EntityIdLinked[index] = entity;
            ++m_CurrentSize;
            return { index, component };
        }

        void RemoveComponentAtIndex(Index::PageIndexOfComponent index)
        {
            LECS_ASSERT(HasComponentAtIndex(index) == false, "There are no component at this index");

            DestroyAt(index);
            SetHasComponentAtIndex(index, true);
            m_EntityIdLinked[index] = EntityId::INVALID;
            --m_CurrentSize;
        }

        ComponentType& GetComponentAtIndex(Index::PageIndexOfComponent index)
        {
            LECS_ASSERT(HasComponentAtIndex(index) == false, "There are no component at this index");
			LECS_ASSERT(m_EntityIdLinked[index] != EntityId::INVALID, "Not supposed to have a valid component linked to a non valid entityId");

            return *reinterpret_cast<ComponentType*>(&m_Page[index]);
		}

        const ComponentType& GetComponentAtIndex(Index::PageIndexOfComponent index) const
        {
            LECS_ASSERT(HasComponentAtIndex(index) == false, "There are no component at this index");
			LECS_ASSERT(m_EntityIdLinked[index] != EntityId::INVALID, "Not supposed to have a valid component linked to a non valid entityId");

            return *reinterpret_cast<ComponentType*>(&m_Page[index]);
		}

        inline EntityId GetEntityIdAtIndex(Index::PageIndexOfComponent index) const
		{
			return m_EntityIdLinked[index];
        }
    };
}
 