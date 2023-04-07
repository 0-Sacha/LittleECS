#pragma once

#include "CompressedComponentStoragePage.h"

#include "CompressedCSMapEntityToComponent.h"
#include "CompressedCSInlineEntityToComponent.h"


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
		static constexpr Index::GlobalIndexOfComponent PAGE_SIZE = ComponentStorageInfo<ComponentType>::PAGE_SIZE;

        using PageType = CompressedComponentStoragePage<ComponentType, PAGE_SIZE>;
        using PageTypeRef = std::unique_ptr<PageType>;
		using PagesContainer = std::vector<PageTypeRef>;
        using FreePages = std::set<Index::IndexOfPage>;

        using CompressedCSInlineEntityToComponentType = CompressedCSInlineEntityToComponent<ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF, PAGE_SIZE>;
        using EntityToComponent = std::conditional_t<ComponentStorageInfo<ComponentType>::USE_MAP_VERSION,
                                                        CompressedCSMapEntityToComponent,
                                                        CompressedCSInlineEntityToComponentType>;

	public:
		~CompressedComponentStorage() override {}

	protected:
		PagesContainer m_PageContainer;
        FreePages m_FreePages;
		EntityToComponent m_EntityToComponent;

    public:
    	bool EntityHasThisComponent(EntityId entity) const override
		{
			return m_EntityToComponent.HasEntity(entity);
		}

    private:
        Index::IndexOfPage GetFreePageIndexOrCreateIt()
        {
            if (m_FreePages.size() == 0)
            {
				m_PageContainer.emplace_back(new PageType);
                Index::IndexOfPage indexOfPage = m_PageContainer.size() - 1;
                m_FreePages.insert(indexOfPage);
				return indexOfPage;
            }

            Index::IndexOfPage indexOfPage = *m_FreePages.begin();
            return indexOfPage;
        }

    public:
        void RemoveComponentOfEntity(EntityId entity) override
        {
            Index::IndexInfo indexInfo = m_EntityToComponent.GetIndexInfoOfEntity(entity);
            m_EntityToComponent.RemoveIndexInfoForEntity(entity);
            PageTypeRef& page = m_PageContainer[indexInfo.IndexOfPage];
            page->RemoveComponentAtIndex(indexInfo.PageIndexOfComponent);

			m_FreePages.insert(indexInfo.IndexOfPage);
        }

	public:
        template <typename... Args>
        ComponentType& AddComponentToEntity(EntityId entity, Args&&... args)
        {
            LECS_ASSERT(EntityHasThisComponent(entity) == false);
            
            Index::IndexOfPage indexOfFreePage = GetFreePageIndexOrCreateIt();
            PageTypeRef& page = m_PageContainer[indexOfFreePage];
            auto [pageIndexOfComponent, component] = page->AddComponent(entity, std::forward<Args>(args)...);
            m_EntityToComponent.AddIndexInfoForEntity(entity, Index::IndexInfo { .IndexOfPage = indexOfFreePage, .PageIndexOfComponent = pageIndexOfComponent });

            if (page->CanAddComponent() == false)
                m_FreePages.erase(indexOfFreePage);

            return component;
        }

        ComponentType& GetComponentOfEntity(EntityId entity)
        {
            Index::IndexInfo indexInfo = m_EntityToComponent.GetIndexInfoOfEntity(entity);
            
            LECS_ASSERT(indexInfo.IndexOfPage < m_PageContainer.size(), "Entity doesn't have this component");

            PageTypeRef& page = m_PageContainer[indexInfo.IndexOfPage];
            return page->GetComponentAtIndex(indexInfo.PageIndexOfComponent);
        }

        const ComponentType& GetComponentOfEntity(EntityId entity) const
        {
            Index::IndexInfo indexInfo = m_EntityToComponent.GetIndexInfoOfEntity(entity);

			LECS_ASSERT(indexInfo.IndexOfPage < m_PageContainer.size(), "Entity doesn't have this component");

            PageTypeRef& page = m_PageContainer[indexInfo.IndexOfPage];
            return page->GetComponentAtIndex(indexInfo.PageIndexOfComponent);
        }

    public:
        // Function = std::function<void(EntityId, ComponentType&)>
        template <typename Function>
    	void ForEachUniqueComponent(Function&& function)
        requires (ComponentStorageInfo<ComponentType>::SEND_ENTITIES_POOL_ON_EACH == false);
        
        // Function = std::function<void(EntityId, const ComponentType&)>
        template <typename Function>
		void ForEachUniqueComponent(Function&& function) const
        requires (ComponentStorageInfo<ComponentType>::SEND_ENTITIES_POOL_ON_EACH == false);
    };
}

#include "CCSEach.h"
