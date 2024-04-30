#pragma once

#include "FastComponentStoragePage.h"
#include "LittleECS/Detail/Exception.h"

#include <memory>
#include <any>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace LECS::Detail
{

    template <typename ComponentType>
    requires (TypeValidForComponentStorage<ComponentType>::Value)
    class FastComponentStorage : public IComponentStorage
    {
    public:
        using M_Type = FastComponentStorage<ComponentType>;

		static constexpr Index::GlobalIndexOfComponent PAGE_SIZE = ComponentStorageInfo<ComponentType>::PAGE_SIZE;

        using PageType = FastComponentStoragePage<ComponentType, PAGE_SIZE>;
        using PageTypeRef = std::unique_ptr<PageType>;
		using PagesContainer = std::vector<PageTypeRef>;
        using AliveEntitiesContainerType = std::vector<typename EntityId::Type>;
        using AliveEntitiesContainer = std::conditional_t<ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF, AliveEntitiesContainerType, int>;

        class FCSErrorCantForEachOnNonREFContainer : public LECSException {};

	public:
		FastComponentStorage()
			: m_PageContainer()
			, m_AliveEntitiesContainer()
		{
		}

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
    	bool HasThisComponent(EntityId entity) const
		{
            Index::IndexInfo indexInfo = GetIndexInfoOfEntity(entity);

            if (indexInfo.IndexOfPage >= m_PageContainer.size())
                return false;
            if (m_PageContainer[indexInfo.IndexOfPage] == nullptr)
                return false;

			return m_PageContainer[indexInfo.IndexOfPage]->HasEntityAtIndex(indexInfo.PageIndexOfComponent);
		}

        bool HasThisComponentV(EntityId entity) const override
		{
            return HasThisComponent(entity);
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

        void RemoveComponentOfEntityV(EntityId entity) override
        {
            return RemoveComponentOfEntity(entity);
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
			LECS_ASSERT(indexInfo.IndexOfPage < m_PageContainer.size(), "Entity doesn't have this component")
            PageTypeRef& page = m_PageContainer[indexInfo.IndexOfPage];
            return page->GetComponentAtIndex(indexInfo.PageIndexOfComponent);
        }

        const ComponentType& GetComponentOfEntity(EntityId entity) const
        {
            Index::IndexInfo indexInfo = GetIndexInfoOfEntity(entity);
			LECS_ASSERT(indexInfo.IndexOfPage < m_PageContainer.size(), "Entity doesn't have this component")
            const PageTypeRef& page = m_PageContainer[indexInfo.IndexOfPage];
            return page->GetComponentAtIndex(indexInfo.PageIndexOfComponent);
        }

        ComponentType* GetComponentOfEntityPtr(EntityId entity)
        {
            Index::IndexInfo indexInfo = GetIndexInfoOfEntity(entity);
            if (indexInfo.IndexOfPage >= m_PageContainer.size())
                return nullptr;
            PageTypeRef& page = m_PageContainer[indexInfo.IndexOfPage];
            return page->GetComponentAtIndexPtr(indexInfo.PageIndexOfComponent);
        }

        const ComponentType* GetComponentOfEntityPtr(EntityId entity) const
        {
            Index::IndexInfo indexInfo = GetIndexInfoOfEntity(entity);
            if (indexInfo.IndexOfPage >= m_PageContainer.size())
                return nullptr;
            const PageTypeRef& page = m_PageContainer[indexInfo.IndexOfPage];
            return page->GetComponentAtIndexPtr(indexInfo.PageIndexOfComponent);
        }

        const void* GetComponentAliasedPtrV(EntityId entity) const override
        {
            return reinterpret_cast<const void*>(GetComponentOfEntityPtr(entity));
        }
		void* GetComponentAliasedPtrV(EntityId entity) override
        {
            return reinterpret_cast<void*>(GetComponentOfEntityPtr(entity));
        }
    
    public:
        // Function = std::function<void(EntityId, ComponentType&)>
        template <typename Function>
        void ForEachStorage(Function&& function)
        requires (ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF && ComponentStorageInfo<ComponentType>::SEND_ENTITIES_POOL_ON_EACH == false);

        // Function = std::function<void(EntityId, const ComponentType&)>
        template <typename Function>
		void ForEachStorage(Function&& function) const
        requires (ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF && ComponentStorageInfo<ComponentType>::SEND_ENTITIES_POOL_ON_EACH == false);
        
        // Function = std::function<void(EntityId, ComponentType&)>
        template <typename Function>
        void ForEachStorage(Function&& function, const auto& registryAliveEntities)
        requires (ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF == false && ComponentStorageInfo<ComponentType>::SEND_ENTITIES_POOL_ON_EACH);

        // Function = std::function<void(EntityId, const ComponentType&)>
        template <typename Function>
		void ForEachStorage(Function&& function, const auto& registryAliveEntities) const
        requires (ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF == false && ComponentStorageInfo<ComponentType>::SEND_ENTITIES_POOL_ON_EACH);

    public:
        decltype(auto) EntitiesIteratorBegin() const
        requires (ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF && ComponentStorageInfo<ComponentType>::SEND_ENTITIES_POOL_ON_EACH == false);
        decltype(auto) EntitiesIteratorEnd() const
        requires (ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF && ComponentStorageInfo<ComponentType>::SEND_ENTITIES_POOL_ON_EACH == false);
        decltype(auto) EntitiesIteratorBegin(const auto& registryAliveEntities) const
        requires (ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF == false && ComponentStorageInfo<ComponentType>::SEND_ENTITIES_POOL_ON_EACH);
        decltype(auto) EntitiesIteratorEnd(const auto& registryAliveEntities) const
        requires (ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF == false && ComponentStorageInfo<ComponentType>::SEND_ENTITIES_POOL_ON_EACH);
    };
}
 
#include "FCSEach.h"
#include "FCSIterator.h"
