#pragma once

#include "FastComponentStoragePage.h"
#include "LittleECS/Detail/Exception.h"

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

        class FCSErrorCantForEachOnNonREFContainer : public LECSException {};

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

            if (indexInfo.IndexOfPage >= m_PageContainer.size())
                return false;
            if (m_PageContainer[indexInfo.IndexOfPage] == nullptr)
                return false;

			return m_PageContainer[indexInfo.IndexOfPage]->HasEntityAtIndex(indexInfo.PageIndexOfComponent);
		}
        
        void RemoveComponentOfEntity(EntityId entity) override
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

        ComponentType& GetComponentOfEntity(EntityId entity)
        {
            Index::IndexInfo indexInfo = GetIndexInfoOfEntity(entity);
            PageTypeRef& page = m_PageContainer[indexInfo.IndexOfPage];
            return page->GetComponentAtIndex(indexInfo.PageIndexOfComponent);
        }

        const ComponentType& GetComponentOfEntity(EntityId entity) const
        {
            Index::IndexInfo indexInfo = GetIndexInfoOfEntity(entity);
            const PageTypeRef& page = m_PageContainer[indexInfo.IndexOfPage];
            return page->GetComponentAtIndex(indexInfo.PageIndexOfComponent);
        }
    
    public:
        void ForEachUniqueComponent(std::function<void(EntityId, ComponentType&)> function)
        requires (ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF)
        {
            for (EntityId entity : m_AliveEntitiesContainer)
            {
                ComponentType& component = GetComponentOfEntity(entity);
                function(entity, component);
            }
        }

		void ForEachUniqueComponent(std::function<void(EntityId, const ComponentType&)> function) const
        requires (ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF)
        {
            for (EntityId entity : m_AliveEntitiesContainer)
            {
                const ComponentType& component = GetComponentOfEntity(entity);
                function(entity, component);
            }
        }

        
        void ForEachUniqueComponent(std::function<void(EntityId, ComponentType&)> function, const std::set<typename EntityId::Type>& registryAliveEntities)
        requires (ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF == false)
        {
            for (EntityId entity : registryAliveEntities)
            {
                Index::IndexInfo indexInfo = GetIndexInfoOfEntity(entity);

                if (indexInfo.IndexOfPage >= m_PageContainer.size())
                    continue;
                if (m_PageContainer[indexInfo.IndexOfPage] == nullptr)
                    continue;

			    if (m_PageContainer[indexInfo.IndexOfPage]->HasEntityAtIndex(indexInfo.PageIndexOfComponent) == false)
                    continue;
                
                ComponentType& component = m_PageContainer[indexInfo.IndexOfPage]->GetComponentAtIndex(entity);
                function(entity, component);
            }
        }

		void ForEachUniqueComponent(std::function<void(EntityId, const ComponentType&)> function, const std::set<typename EntityId::Type>& registryAliveEntities) const
        requires (ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF == false)
        {
            for (EntityId entity : registryAliveEntities)
            {
                Index::IndexInfo indexInfo = GetIndexInfoOfEntity(entity);

                if (indexInfo.IndexOfPage >= m_PageContainer.size())
                    continue;
                if (m_PageContainer[indexInfo.IndexOfPage] == nullptr)
                    continue;

			    if (m_PageContainer[indexInfo.IndexOfPage]->HasEntityAtIndex(indexInfo.PageIndexOfComponent) == false)
                    continue;
                
                ComponentType& component = m_PageContainer[indexInfo.IndexOfPage]->GetComponentAtIndex(entity);
                function(entity, component);
            }
        }
    };
}
 