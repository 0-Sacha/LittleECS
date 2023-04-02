#pragma once

#include "FastComponentStoragePage.h"

#include <memory>
#include <any>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace LittleECS::Detail
{

    template <typename ComponentType>
    class FastComponentStorage : public IComponentStorage
    {
    public:
		static constexpr Index::GlobalIndexOfComponent PAGE_SIZE = ComponentStorageInfo<ComponentType>::PAGE_SIZE;

        using PageType = FastComponentStoragePage<ComponentType, PAGE_SIZE>;
        using PageTypeRef = std::unique_ptr<PageType>;
		using PagesContainer = std::vector<PageTypeRef>;
        using AliveEntitiesContainerType = std::vector<typename EntityId::Type>;
        using AliveEntitiesContainer = std::conditional_t<ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF, AliveEntitiesContainerType, int>;

	public:
		~FastComponentStorage() override
        {
            if constexpr (ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF)
                m_AliveEntitiesContainer.reserve(PAGE_SIZE);
        }

	protected:
		PagesContainer m_PageContainer;
        AliveEntitiesContainer m_AliveEntitiesContainer;

	protected:
		inline Index::IndexInfo GetIndexInfoOfEntity(EntityId entity) const
		{
			Index::IndexInfo indexInfo;
			indexInfo.IndexOfPage = entity.Id / PAGE_SIZE;
			indexInfo.PageIndexOfComponent = entity.Id % PAGE_SIZE;
			return indexInfo;
		}

    public:
    	bool EntityHasThisComponent(EntityId entity) const override
		{
            Index::IndexInfo indexInfo = GetIndexInfoOfEntity(entity);

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
            Index::IndexInfo indexInfo = GetIndexInfoOfEntity(entity);

            if (indexInfo.IndexOfPage >= m_PageContainer.size())
            {
                m_PageContainer.resize(indexInfo.IndexOfPage + 1);
            }

            PageTypeRef& page = m_PageContainer[indexInfo.IndexOfPage];

            if (page == nullptr)
            {
                page = std::make_unique<PageType>();
            }

            if constexpr (ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF)
            {
                m_AliveEntitiesContainer.emplace_back(entity.Id);
                return page->AddComponent(entity, indexInfo.PageIndexOfComponent, m_AliveEntitiesContainer.size() - 1, std::forward<Args>(args)...);
            }
            else
                return page->AddComponent(entity, indexInfo.PageIndexOfComponent, std::forward<Args>(args)...);
        }

        void RemoveComponentOfEntity(EntityId entity)
        {
            Index::IndexInfo indexInfo = GetIndexInfoOfEntity(entity);
            PageTypeRef& page = m_PageContainer[indexInfo.IndexOfPage];

            if constexpr (ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF)
            {
                Index::IndexInAliveList indexInAliveList = page->GetIndexInAliveListAtIndex(indexInfo.PageIndexOfComponent);
                typename EntityId::Type lastEntity = m_AliveEntitiesContainer.back();
                m_AliveEntitiesContainer[indexInAliveList] = lastEntity;
                m_AliveEntitiesContainer.pop_back();
            }

            page->RemoveComponentAtIndex(indexInfo.PageIndexOfComponent);
        }

        ComponentType& GetComponentOfEntity(EntityId entity)
        {
            Index::IndexInfo indexInfo = GetIndexInfoOfEntity(entity);
            PageTypeRef& page = m_PageContainer[indexInfo.IndexOfPage];
            return page->GetComponentAtIndex(indexInfo.PageIndexOfComponent);
        }

        const ComponentType& GetComponentOfEntity(EntityId entity) const
        {
            Index::IndexInfo indexInfo = GetIndexInfoOfEntity(entity);
            PageTypeRef& page = m_PageContainer[indexInfo.IndexOfPage];
            return page->GetComponentAtIndex(indexInfo.PageIndexOfComponent);
        }
    };
}
 