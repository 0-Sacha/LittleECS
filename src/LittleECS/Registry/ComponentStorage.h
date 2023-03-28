#pragma once

#include "LittleECS/Detail/ComponentId.h"
#include "LittleECS/Detail/EntityId.h"

#include "LittleECS/Detail/ComponentIdGenerator.h"

#include <map>
#include <set>
#include <array>
#include <any>

namespace LittleECS::Detail
{
    class BasicComponentStorage
    {
	public:
        virtual ~BasicComponentStorage() = default;

	public:
        using GlobalIndexOfComponent = std::size_t;
        using PageIndexOfComponent = std::size_t;
        using IndexOfPage = std::size_t;

        struct IndexInfo
        {
            PageIndexOfComponent PageIndexOfComponent;
            IndexOfPage IndexOfPage;
        };

        using EntityToComponent = std::map<typename EntityId::Type, GlobalIndexOfComponent>;


	public:
		bool EntityHasThisComponent(EntityId entity) const
		{
			return m_EntityToComponent.contains(entity.Id);
		}

	public:
		virtual void ForEach(std::any function) = 0;
		virtual void ForEach(std::any function) const = 0;

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
                std::uint8_t StorageData[sizeof(ComponentType)];
                ComponentType ComponentValue;
            };
            DataStorageType Data{};
        };

        static constexpr std::size_t NUMBER_OF_BLOCKS = PAGE_SIZE / sizeof(std::size_t);

	private:
        template <typename... Args>
		inline ComponentType& ConstructAt(BasicComponentStorage::PageIndexOfComponent index, Args&&... args)
		{
			ComponentDataBuffer* buffer = &m_Page[index];
			ComponentType* component = new (buffer) ComponentType(std::forward<Args>(args)...);
            return *component;
		}

        inline void DestroyAt(BasicComponentStorage::PageIndexOfComponent index)
        {
            ComponentType& component = m_Page[index].Data.ComponentValue;
            component.~ComponentType();
        }

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

            for (std::size_t i = 0; i < PAGE_SIZE; ++i)
                m_EntityIdLinked[i] = EntityId::NON_VALID;
        }

		~ComponentsStoragePage()
		{
			ForEach([this](BasicComponentStorage::PageIndexOfComponent index) { this->DestroyAt(index); });
		}

    protected:
        std::array<ComponentDataBuffer, PAGE_SIZE> m_Page;
		std::size_t m_FreeComponent[PAGE_SIZE / sizeof(std::size_t)];
		EntityId m_EntityIdLinked[PAGE_SIZE];
        std::size_t m_CurrentSize;

    public:
        inline bool HasComponentAt(BasicComponentStorage::PageIndexOfComponent index) const
        {
            std::size_t indexOfBlock = index / (sizeof(std::size_t) * 8);
            std::size_t block = *(m_FreeComponent + indexOfBlock);
            std::size_t indexInBlock = index % (sizeof(std::size_t) * 8);
            return block & (static_cast<std::size_t>(1) << indexInBlock);
        }

    private:
        inline void SetHasComponentAt(BasicComponentStorage::PageIndexOfComponent index, bool set)
        {
            std::size_t indexOfBlock = index / (sizeof(std::size_t) * 8);
            std::size_t* block = m_FreeComponent + indexOfBlock;
            std::size_t indexInBlock = index % (sizeof(std::size_t) * 8);

            *block = *block & ~(static_cast<std::size_t>(1) << indexInBlock);

            if (set)
                *block |= (static_cast<std::size_t>(1) << indexInBlock);
        }

        BasicComponentStorage::PageIndexOfComponent GetNextIndex() const
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
        inline ComponentType& GetComponentAtIndex(BasicComponentStorage::PageIndexOfComponent index)
        {
            LECS_ASSERT(HasComponentAt(index));
            return m_Page[index].Data.ComponentValue;
        }

		inline const ComponentType& GetComponentAtIndex(BasicComponentStorage::PageIndexOfComponent index) const
		{
			LECS_ASSERT(HasComponentAt(index));
			return m_Page[index].Data.ComponentValue;
		}

		inline EntityId GetEntityIdAtIndex(BasicComponentStorage::PageIndexOfComponent index) const
		{
			return m_EntityIdLinked[index];
        }

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
                    BasicComponentStorage::PageIndexOfComponent blockShift = (blockIndex * sizeof(std::size_t) * 8);

                    std::size_t block = *beginFreeListBlocks;
                    std::size_t mask = 1;
                    std::uint8_t freeIndexInBlock = 0;
                    for (; freeIndexInBlock < sizeof(std::size_t) * 8; ++freeIndexInBlock)
                    {
                        if ((block & mask) == 0)
                        {
                            BasicComponentStorage::PageIndexOfComponent index = freeIndexInBlock + blockShift;
                            
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
        std::pair<BasicComponentStorage::PageIndexOfComponent, ComponentType&> AddComponent(EntityId entity, Args&&... args)
        {
            LECS_ASSERT(CanAddComponent(), "Can't add more component to this page");

            BasicComponentStorage::PageIndexOfComponent index = GetNextIndex();
            ComponentType& component = ConstructAt(index, std::forward<Args>(args)...);
            SetHasComponentAt(index, false);
            m_EntityIdLinked[index] = entity;
            ++m_CurrentSize;
            return { index, component };
        }

        void RemoveComponentAt(BasicComponentStorage::PageIndexOfComponent index)
        {
            LECS_ASSERT(HasComponentAt(index) == false, "There are no component at this index");

            DestroyAt(index);
            SetHasComponentAt(index, true);
            m_EntityIdLinked[index] = EntityId::NON_VALID;
            --m_CurrentSize;
        }

        ComponentType& GetComponentAt(BasicComponentStorage::PageIndexOfComponent index)
        {
            LECS_ASSERT(HasComponentAt(index) == false, "There are no component at this index");
			LECS_ASSERT(m_EntityIdLinked[index] != EntityId::NON_VALID, "Not supposed to have a valid component linked to a non valid entityId");

            return *reinterpret_cast<ComponentType*>(&m_Page[index]);
		}

        const ComponentType& GetComponentAt(BasicComponentStorage::PageIndexOfComponent index) const
        {
            LECS_ASSERT(HasComponentAt(index) == false, "There are no component at this index");
			LECS_ASSERT(m_EntityIdLinked[index] != EntityId::NON_VALID, "Not supposed to have a valid component linked to a non valid entityId");

            return *reinterpret_cast<ComponentType*>(&m_Page[index]);
		}
    };

	template <typename ComponentType>
    struct ComponentStorageInfo
    {
		static constexpr BasicComponentStorage::GlobalIndexOfComponent PAGE_SIZE = 1024;
    };

    template <typename ComponentType>
    class ComponentStorage : public BasicComponentStorage
    {
    public:
		static constexpr GlobalIndexOfComponent PAGE_SIZE = ComponentStorageInfo<ComponentType>::PAGE_SIZE;

        using PageType = ComponentsStoragePage<ComponentType, PAGE_SIZE>;
        using PageTypeRef = std::unique_ptr<PageType>;

		using PagesContainer = std::vector<PageTypeRef>;
        using FreePages = std::set<IndexOfPage>;

	public:
		~ComponentStorage() override
        {
        }

	protected:
		PagesContainer m_PageContainer;
        FreePages m_FreePages;

	protected:
		inline IndexInfo GetIndexInfoOfEntity(EntityId entity) const
		{
			LECS_ASSERT(EntityHasThisComponent(entity));

			GlobalIndexOfComponent indexToComponent = m_EntityToComponent.at(entity.Id);

			IndexInfo indexInfo;
			indexInfo.IndexOfPage = indexToComponent / PAGE_SIZE;
			indexInfo.PageIndexOfComponent = indexToComponent % PAGE_SIZE;
			return indexInfo;
		}

    public:
		void ForEach(std::any functionAliased) override
        {
            std::function<void(EntityId, ComponentType&)> function = std::any_cast<std::function<void(EntityId, ComponentType&)>>(functionAliased);

            for (PageTypeRef& page : m_PageContainer)
            {
                page->ForEach([&page, &function](BasicComponentStorage::PageIndexOfComponent index) { function(page->GetEntityIdAtIndex(index), page->GetComponentAtIndex(index)); });
            }
        }

		void ForEach(std::any functionAliased) const override
		{
			std::function<void(EntityId, const ComponentType&)> function = std::any_cast<std::function<void(EntityId, const ComponentType&)>>(functionAliased);

			for (const PageTypeRef& page : m_PageContainer)
			{
				page->ForEach([&page, &function](BasicComponentStorage::PageIndexOfComponent index) { function(page->GetEntityIdAtIndex(index), page->GetComponentAtIndex(index)); });
			}
		}

	private:
        BasicComponentStorage::IndexOfPage GetFreePageIndexOrCreateIt()
        {
            if (m_FreePages.size() == 0)
            {
				m_PageContainer.emplace_back(new PageType);
                IndexOfPage indexOfPage = m_PageContainer.size() - 1;
                m_FreePages.insert(indexOfPage);
				return indexOfPage;
            }

            IndexOfPage indexOfPage = *m_FreePages.begin();
            return indexOfPage;
        }

	public:
        template <typename... Args>
        ComponentType& AddComponentToEntity(EntityId entity, Args&&... args)
        {
            LECS_ASSERT(EntityHasThisComponent(entity) == false);
            
            IndexOfPage indexOfFreePage = GetFreePageIndexOrCreateIt();
            PageTypeRef& page = m_PageContainer[indexOfFreePage];
            auto [index, component] = page->AddComponent(entity, std::forward<Args>(args)...);
            index += indexOfFreePage * PAGE_SIZE;
            m_EntityToComponent[entity] = index;

            if (page->CanAddComponent() == false)
                m_FreePages.erase(indexOfFreePage);

            return component;
        }

        void RemoveComponentOfEntity(EntityId entity)
        {
            IndexInfo indexInfo = GetIndexInfoOfEntity(entity);
            PageTypeRef& page = m_PageContainer[indexInfo.IndexOfPage];
            page->RemoveComponentAt(indexInfo.PageIndexOfComponent);

			if (page->CanAddComponent() == true)
				m_FreePages.insert(indexInfo.IndexOfPage);
        }

        ComponentType& GetComponentOfEntity(EntityId entity)
        {
            IndexInfo indexInfo = GetIndexInfoOfEntity(entity);
            PageType* page = m_PageContainer[indexInfo.IndexOfPage].get();
            return page->GetComponentAt(indexInfo.PageIndexOfComponent);
        }

        const ComponentType& GetComponentOfEntity(EntityId entity) const
        {
            IndexInfo indexInfo = GetIndexInfoOfEntity(entity);
            PageType* page = m_PageContainer[indexInfo.IndexOfPage].get();
            return page->GetComponentAt(indexInfo.PageIndexOfComponent);
        }
    };
}
 