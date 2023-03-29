#pragma once

#include "FastComponentStoragePage.h"

#include <memory>
#include <any>
#include <unordered_map>
#include <set>
#include <vector>

namespace LittleECS::Detail
{

    template <typename ComponentType>
    class FastComponentStorage : public IComponentStorage
    {
    public:
		static constexpr GlobalIndexOfComponent PAGE_SIZE = ComponentStorageInfo<ComponentType>::PAGE_SIZE;

        using PageType = FastComponentStoragePage<ComponentType, PAGE_SIZE>;
        using PageTypeRef = std::unique_ptr<PageType>;
		using PagesContainer = std::vector<PageTypeRef>;

	public:
		~FastComponentStorage() override
        {
        }

	protected:
		PagesContainer m_PageContainer;

	protected:
		inline IndexInfo GetIndexInfoOfEntity(EntityId entity) const
		{
			IndexInfo indexInfo;
			indexInfo.IndexOfPage = entity.Id / PAGE_SIZE;
			indexInfo.PageIndexOfComponent = entity.Id % PAGE_SIZE;
			return indexInfo;
		}

    public:
    	bool EntityHasThisComponent(EntityId entity) const override
		{
            IndexInfo indexInfo = GetIndexInfoOfEntity(entity);

            LECS_ASSERT(indexInfo.IndexOfPage < m_PageContainer.size(), "This entity can't be in this storage");
            LECS_ASSERT(m_PageContainer[indexInfo.IndexOfPage] != nullptr, "This entity can't be in this storage");

			return m_PageContainer[indexInfo.IndexOfPage]->HasEntityAtIndex(indexInfo.PageIndexOfComponent);
		}

        void ForEach(std::any function) override {}
		void ForEach(std::any function) const override {}

	public:
        template <typename... Args>
        ComponentType& AddComponentToEntity(EntityId entity, Args&&... args)
        {
            IndexInfo indexInfo = GetIndexInfoOfEntity(entity);

            if (indexInfo.IndexOfPage >= m_PageContainer.size())
            {
                m_PageContainer.resize(indexInfo.IndexOfPage + 1);
            }

            PageTypeRef& page = m_PageContainer[indexInfo.IndexOfPage];

            if (page == nullptr)
            {
                page = std::make_unique<PageType>();
            }

            return page->AddComponent(entity, indexInfo.PageIndexOfComponent, std::forward<Args>(args)...);
        }

        void RemoveComponentOfEntity(EntityId entity)
        {
            IndexInfo indexInfo = GetIndexInfoOfEntity(entity);
            PageTypeRef& page = m_PageContainer[indexInfo.IndexOfPage];
            page->RemoveComponentAtIndex(indexInfo.PageIndexOfComponent);
        }

        ComponentType& GetComponentOfEntity(EntityId entity)
        {
            IndexInfo indexInfo = GetIndexInfoOfEntity(entity);
            PageTypeRef& page = m_PageContainer[indexInfo.IndexOfPage];
            return page->GetComponentAtIndex(indexInfo.PageIndexOfComponent);
        }

        const ComponentType& GetComponentOfEntity(EntityId entity) const
        {
            IndexInfo indexInfo = GetIndexInfoOfEntity(entity);
            PageTypeRef& page = m_PageContainer[indexInfo.IndexOfPage];
            return page->GetComponentAtIndex(indexInfo.PageIndexOfComponent);
        }
    };
}
 