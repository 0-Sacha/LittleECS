#pragma once

#include "CompressedComponentStoragePage.h"

#include <memory>
#include <any>
#include <unordered_map>
#include <set>
#include <vector>

namespace LittleECS::Detail
{

    template <typename ComponentType>
    class CompressedComponentStorage : public IComponentStorage
    {
    public:
		static constexpr GlobalIndexOfComponent PAGE_SIZE = ComponentStorageInfo<ComponentType>::PAGE_SIZE;

        using PageType = CompressedComponentStoragePage<ComponentType, PAGE_SIZE>;
        using PageTypeRef = std::unique_ptr<PageType>;
		using PagesContainer = std::vector<PageTypeRef>;
        using FreePages = std::set<IndexOfPage>;
        using EntityToComponent = std::unordered_map<typename EntityId::Type, GlobalIndexOfComponent>;

	public:
		~CompressedComponentStorage() override
        {
        }

	protected:
		PagesContainer m_PageContainer;
        FreePages m_FreePages;
		EntityToComponent m_EntityToComponent;

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
    	bool EntityHasThisComponent(EntityId entity) const override
		{
			return m_EntityToComponent.contains(entity.Id);
		}

		void ForEach(std::any functionAliased) override
        {
            std::function<void(EntityId, ComponentType&)> function = std::any_cast<std::function<void(EntityId, ComponentType&)>>(functionAliased);

            for (PageTypeRef& page : m_PageContainer)
            {
                page->ForEach([&page, &function](IComponentStorage::PageIndexOfComponent index) { function(page->GetEntityIdAtIndex(index), page->GetComponentAtIndex(index)); });
            }
        }

		void ForEach(std::any functionAliased) const override
		{
			std::function<void(EntityId, const ComponentType&)> function = std::any_cast<std::function<void(EntityId, const ComponentType&)>>(functionAliased);

			for (const PageTypeRef& page : m_PageContainer)
			{
				page->ForEach([&page, &function](IComponentStorage::PageIndexOfComponent index) { function(page->GetEntityIdAtIndex(index), page->GetComponentAtIndex(index)); });
			}
		}

	private:
        IComponentStorage::IndexOfPage GetFreePageIndexOrCreateIt()
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
            PageTypeRef& page = m_PageContainer[indexInfo.IndexOfPage];
            return page->GetComponentAt(indexInfo.PageIndexOfComponent);
        }

        const ComponentType& GetComponentOfEntity(EntityId entity) const
        {
            IndexInfo indexInfo = GetIndexInfoOfEntity(entity);
            PageTypeRef& page = m_PageContainer[indexInfo.IndexOfPage];
            return page->GetComponentAt(indexInfo.PageIndexOfComponent);
        }
    };
}
 