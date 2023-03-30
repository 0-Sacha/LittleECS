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
        
    protected:
        class EntityToComponent
        {
        public:
            using Bucket = std::array<IndexInfo, PAGE_SIZE>;
            using BucketRef = std::unique_ptr<Bucket>;
            using BucketContainer = std::vector<BucketRef>;

        private:
            BucketContainer m_BucketContainer;

        private:
            inline IndexInfo GetBucketInfoOfEntity(EntityId entity) const
            {
                IndexInfo indexInfo;
                indexInfo.IndexOfPage = entity.Id / PAGE_SIZE;
                indexInfo.PageIndexOfComponent = entity.Id % PAGE_SIZE;
                return indexInfo;
            }

        public:
            inline bool HasEntity(EntityId entity) const
            {
                IndexInfo bucketInfo = GetBucketInfoOfEntity(entity);

                if (bucketInfo.IndexOfPage >= m_BucketContainer.size())
                    return false;

                const BucketRef& bucketRef = m_BucketContainer[bucketInfo.IndexOfPage];

                if (bucketRef == nullptr)
                    return false;

                IndexInfo indexInfo = (*bucketRef)[bucketInfo.PageIndexOfComponent];

                return indexInfo.IsValid();
            }

            inline IndexInfo GetIndexInfoOfEntity(EntityId entity) const
            {
                IndexInfo bucketInfo = GetBucketInfoOfEntity(entity);
             
                LECS_ASSERT(bucketInfo.IndexOfPage < m_BucketContainer.size(), "This container can have this entity");

                const BucketRef& bucketRef = m_BucketContainer[bucketInfo.IndexOfPage];

                LECS_ASSERT(bucketRef != nullptr, "This container can have this entity");

                return (*bucketRef)[bucketInfo.PageIndexOfComponent];
            }

            inline void AddIndexInfoForEntity(EntityId entity, IndexInfo indexInfo)
            {
                IndexInfo bucketInfo = GetBucketInfoOfEntity(entity);

                if (bucketInfo.IndexOfPage >= m_BucketContainer.size())
                {
                    m_BucketContainer.resize(bucketInfo.IndexOfPage + 1);
                }

                BucketRef& bucketRef = m_BucketContainer[bucketInfo.IndexOfPage];

                if (bucketRef == nullptr)
                {
                    bucketRef = std::make_unique<Bucket>();
                    Bucket& bucket = *bucketRef;
                    for (std::size_t i = 0; i < PAGE_SIZE; ++i)
                        bucket[i].SetInvalid();
                }

                (*bucketRef)[bucketInfo.PageIndexOfComponent] = indexInfo;
            }

            inline void RemoveIndexInfoForEntity(EntityId entity, IndexInfo indexInfo)
            {
                IndexInfo bucketInfo = GetBucketInfoOfEntity(entity);

                LECS_ASSERT(bucketInfo.IndexOfPage < m_BucketContainer.size(), "This container can have this entity");

                BucketRef& bucketRef = m_BucketContainer[bucketInfo.IndexOfPage];

                LECS_ASSERT(bucketRef != nullptr, "This container can have this entity");

                (*bucketRef)[bucketInfo.PageIndexOfComponent].SetInvalid();
            }
        };

	public:
		~CompressedComponentStorage() override
        {
        }

	protected:
		PagesContainer m_PageContainer;
        FreePages m_FreePages;
		EntityToComponent m_EntityToComponent;

    public:
    	bool EntityHasThisComponent(EntityId entity) const override
		{
			return m_EntityToComponent.HasEntity(entity);
		}

		void ForEach(std::any functionAliased) override
        {
        //    std::function<void(EntityId, ComponentType&)> function = std::any_cast<std::function<void(EntityId, ComponentType&)>>(functionAliased);
        //
        //    for (PageTypeRef& page : m_PageContainer)
        //    {
        //        page->ForEach([&page, &function](IComponentStorage::PageIndexOfComponent index) { function(page->GetEntityIdAtIndex(index), page->GetComponentAtIndex(index)); });
        //    }
        }

		void ForEach(std::any functionAliased) const override
		{
		// 	std::function<void(EntityId, const ComponentType&)> function = std::any_cast<std::function<void(EntityId, const ComponentType&)>>(functionAliased);
        // 
		// 	for (const PageTypeRef& page : m_PageContainer)
		// 	{
		// 		page->ForEach([&page, &function](IComponentStorage::PageIndexOfComponent index) { function(page->GetEntityIdAtIndex(index), page->GetComponentAtIndex(index)); });
		// 	}
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
            auto [pageIndexOfComponent, component] = page->AddComponent(entity, std::forward<Args>(args)...);
            m_EntityToComponent.AddIndexInfoForEntity(entity, IndexInfo { .IndexOfPage = indexOfFreePage, .PageIndexOfComponent = pageIndexOfComponent });

            if (page->CanAddComponent() == false)
                m_FreePages.erase(indexOfFreePage);

            return component;
        }

        void RemoveComponentOfEntity(EntityId entity)
        {
            IndexInfo indexInfo = m_EntityToComponent.GetIndexInfoOfEntity(entity);
            m_EntityToComponent.RemoveIndexInfoForEntity(entity);
            PageTypeRef& page = m_PageContainer[indexInfo.IndexOfPage];
            page->RemoveComponentAtIndex(indexInfo.PageIndexOfComponent);

			m_FreePages.insert(indexInfo.IndexOfPage);
        }

        ComponentType& GetComponentOfEntity(EntityId entity)
        {
            IndexInfo indexInfo = m_EntityToComponent.GetIndexInfoOfEntity(entity);
            
            LECS_ASSERT(indexInfo.IndexOfPage < m_PageContainer.size(), "Entity doesn't have this component");

            PageTypeRef& page = m_PageContainer[indexInfo.IndexOfPage];
            return page->GetComponentAtIndex(indexInfo.PageIndexOfComponent);
        }

        const ComponentType& GetComponentOfEntity(EntityId entity) const
        {
            IndexInfo indexInfo = m_EntityToComponent.GetIndexInfoOfEntity(entity);

			LECS_ASSERT(indexInfo.IndexOfPage < m_PageContainer.size(), "Entity doesn't have this component");

            PageTypeRef& page = m_PageContainer[indexInfo.IndexOfPage];
            return page->GetComponentAtIndex(indexInfo.PageIndexOfComponent);
        }
    };
}
 