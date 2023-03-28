#pragma once

#include "LittleECS/Detail/ComponentId.h"
#include "LittleECS/Detail/EntityId.h"

#include "LittleECS/Detail/ComponentIdGenerator.h"

#include <map>
#include <array>

namespace LittleECS::Detail
{
    class BasicComponentStorage
    {
    public:
        using GlobalIntexOfComponent = std::size_t;
        using PageIntexOfComponent = std::size_t;
        using IntexOfPage = std::size_t;

        struct IndexInfo
        {
            PageIntexOfComponent PageIntexOfComponent;
            IntexOfPage IndexOfPage;
        };

        static constexpr GlobalIntexOfComponent PAGE_SIZE = 1024;

    public:
        using EntityToComponent = std::map<typename EntityId::Type, GlobalIntexOfComponent>;

    protected:
        inline IndexInfo GetIndexInfoOfEntity(EntityId entity) const
        {
            LECS_ASSERT(EntityHasThisComponent(entity));

            GlobalIntexOfComponent indexToComponent = m_EntityToComponent.at(entity.Id);

            IndexInfo indexInfo;
            indexInfo.IndexOfPage = indexToComponent / PAGE_SIZE;
            indexInfo.PageIntexOfComponent = indexToComponent % PAGE_SIZE;
            return indexInfo;
        }

    public:
        bool EntityHasThisComponent(EntityId entity) const
        {
            return m_EntityToComponent.contains(entity.Id);
        }

    protected:
        EntityToComponent m_EntityToComponent;
    };

    template <typename ComponentType, std::size_t PAGE_SIZE>
    requires (PAGE_SIZE % sizeof(std::size_t) == 0)
    class ComponentsStoragePage
    {
    private:
        struct ComponentDataBuffer
        {
            union DataStorageType
            {
                std::uint8_t Data[sizeof(ComponentType)]; 
                ComponentType Type;
            };
            DataStorageType Data{};
        };

        static constexpr std::size_t NUMBER_OF_BLOCKS = PAGE_SIZE / sizeof(std::size_t);

    public:
        ComponentsStoragePage()
            : m_Page()
            , m_CurrentSize()
        {
            std::size_t* beginFreeListBlocks = m_FreeComponent;
            std::size_t* endFreeListBlocks = m_FreeComponent + NUMBER_OF_BLOCKS;
            while (beginFreeListBlocks < endFreeListBlocks)
            {
                *beginFreeListBlocks = std::numeric_limits<std::size_t>::max();
                ++beginFreeListBlocks;
            }
        }

    protected:
        std::array<ComponentDataBuffer, PAGE_SIZE> m_Page;
        std::size_t m_FreeComponent[PAGE_SIZE / sizeof(std::size_t)];
        std::size_t m_CurrentSize;

    private:
        inline bool HasComponentAt(BasicComponentStorage::PageIntexOfComponent index)
        {
            std::size_t indexOfBlock = index / (sizeof(std::size_t) * 8);
            std::size_t* block = m_FreeComponent + indexOfBlock;
            std::size_t indexInBlock = index % (sizeof(std::size_t) * 8);
            return *block & (static_cast<std::size_t>(1) << indexInBlock);
        }

        inline void SetHasComponentAt(BasicComponentStorage::PageIntexOfComponent index, bool set)
        {
            std::size_t indexOfBlock = index / (sizeof(std::size_t) * 8);
            std::size_t* block = m_FreeComponent + indexOfBlock;
            std::size_t indexInBlock = index % (sizeof(std::size_t) * 8);

            *block = *block & ~(static_cast<std::size_t>(1) << indexInBlock);

            if (set)
                *block |= (static_cast<std::size_t>(1) << indexInBlock);
        }

        BasicComponentStorage::PageIntexOfComponent GetNextIndex()
        {
            std::size_t* beginFreeListBlocks = m_FreeComponent;
            std::size_t* endFreeListBlocks = m_FreeComponent + NUMBER_OF_BLOCKS;
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
            uint8_t freeIndexInBlock = 0;
            for(; freeIndexInBlock < sizeof(std::size_t) * 8; ++freeIndexInBlock)
            {
                if (block & mask)
                {
                    break;
                }
                mask = mask << 1;
            }

            LECS_ASSERT(freeIndexInBlock != (sizeof(std::size_t) * 8), "The block found is full");

            std::size_t foundIndex = freeIndexInBlock + (blockIndex * sizeof(std::size_t) * 8);

			// LECS_ASSERT(HasComponentAt(foundIndex) == true, "Impl problem here");

            return foundIndex;
        }

    public:
        inline bool CanAddComponent() const { return m_CurrentSize + 1 < PAGE_SIZE; }

        template<typename... Args>
        std::pair<BasicComponentStorage::PageIntexOfComponent, ComponentType&> AddComponent(Args&&... args)
        {
            LECS_ASSERT(CanAddComponent(), "Can't add more component to this page");

            BasicComponentStorage::PageIntexOfComponent index = GetNextIndex();

            ComponentDataBuffer* buffer = &m_Page[index];
            ComponentType* component = new (buffer) ComponentType(std::forward<Args>(args)...);

            SetHasComponentAt(index, false);
            ++m_CurrentSize;

            return { index, *component };
        }

        void RemoveComponentAt(BasicComponentStorage::PageIntexOfComponent index)
        {
            LECS_ASSERT(HasComponentAt(index) == false, "There are no component at this index");
            ComponentDataBuffer* buffer = m_Page[index];
            buffer->~ComponentType();
            SetHasComponentAt(index, true);
            --m_CurrentSize;
        }

        ComponentType& GetComponentAt(BasicComponentStorage::PageIntexOfComponent index)
        {
            LECS_ASSERT(HasComponentAt(index) == false, "There are no component at this index");
            return *reinterpret_cast<ComponentType*>(&m_Page[index]);
        }

        const ComponentType& GetComponentAt(BasicComponentStorage::PageIntexOfComponent index) const
        {
            LECS_ASSERT(HasComponentAt(index) == false, "There are no component at this index");
            return *reinterpret_cast<ComponentType*>(&m_Page[index]);
        }
    };

    template <typename ComponentType>
    class ComponentStorage : public BasicComponentStorage
    {
    public:
        using PageType = ComponentsStoragePage<ComponentType, PAGE_SIZE>;
        using PageTypeRef = std::unique_ptr<PageType>;
        using PagesContainer = std::vector<PageTypeRef>;

    public:
        template <typename... Args>
        ComponentType& AddComponentToEntity(EntityId entity, Args&&... args)
        {
            LECS_ASSERT(EntityHasThisComponent(entity) == false);
            
            PageTypeRef* freePage = nullptr;
            std::size_t pageIndex = 0;
            for(PageTypeRef& page : m_PageContainer)
            {
                if (page->CanAddComponent())
                {
                    freePage = &page;
                    break;
                }
                ++pageIndex;
            }

            if (freePage == nullptr)
            {
                PageTypeRef& newPage = m_PageContainer.emplace_back(new PageType);
                freePage = &newPage;
                LECS_ASSERT(pageIndex == m_PageContainer.size() - 1);
            }

            PageTypeRef& page = *freePage;
            auto [index, component] = page->AddComponent(std::forward<Args>(args)...);
            index += pageIndex * PAGE_SIZE;
            m_EntityToComponent[entity] = index;
            return component;
        }

        void RemoveComponentOfEntity(EntityId entity)
        {
            IndexInfo indexInfo = GetIndexInfoOfEntity(entity);
            PageType* page = m_PageContainer[indexInfo.IndexOfPage].get();
            return page->RemoveComponentAt(indexInfo.PageIntexOfComponent);
        }

        ComponentType& GetComponentOfEntity(EntityId entity)
        {
            IndexInfo indexInfo = GetIndexInfoOfEntity(entity);
            PageType* page = m_PageContainer[indexInfo.IndexOfPage].get();
            return page->GetComponentAt(indexInfo.PageIntexOfComponent);
        }

        const ComponentType& GetComponentOfEntity(EntityId entity) const
        {
            IndexInfo indexInfo = GetIndexInfoOfEntity(entity);
            PageType* page = m_PageContainer[indexInfo.IndexOfPage].get();
            return page->GetComponentAt(indexInfo.PageIntexOfComponent);
        }

    protected:
        PagesContainer m_PageContainer;
    };
}
 