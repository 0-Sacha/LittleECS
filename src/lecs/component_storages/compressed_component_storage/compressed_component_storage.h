#pragma once

#include "lecs/registry/component_storage.h"
#include "lecs/detail/exception.h"

#include "ccs_page.h"

#include "ccs_entity_to_component_map.h"
#include "ccs_entity_to_component_inline.h"

#include <memory>
#include <any>
#include <unordered_map>
#include <set>
#include <vector>

namespace lecs::detail
{
    template <typename ComponentType>
    requires (TypeValidForComponentStorage<ComponentType>::value)
    class CompressedComponentStorage : public IComponentStorage
    {
    public:
        using M_Type = CompressedComponentStorage<ComponentType>;

        static constexpr Index::GlobalIndexOfComponent PAGE_SIZE = ComponentStorageInfo<ComponentType>::PAGE_SIZE;

        using PageType = CompressedComponentStoragePage<ComponentType, PAGE_SIZE>;
        using PageTypeRef = std::unique_ptr<PageType>;
        using PagesContainer = std::vector<PageTypeRef>;
        using FreePages = std::set<Index::IndexOfPage>;

        using CCS_EntityToComponent_InlineType = CCS_EntityToComponent_Inline<ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF, PAGE_SIZE>;
        using EntityToComponent = std::conditional_t<ComponentStorageInfo<ComponentType>::USE_MAP_VERSION,
                                                        CCS_EntityToComponent_Map,
                                                        CCS_EntityToComponent_InlineType>;

    public:
        ~CompressedComponentStorage() override {}

    protected:
        PagesContainer page_container_;
        FreePages free_pages_;
        EntityToComponent entity_to_component_;

    public:
        const PagesContainer& get_page_container() const
        {
            return page_container_;
        }

        bool has_this_component(EntityId entity) const
        {
            return entity_to_component_.has_entity(entity);
        }

        bool has_this_component_v(EntityId entity) const override
        {
            return has_this_component(entity);
        }

    private:
        Index::IndexOfPage get_free_page_index_or_create_it()
        {
            if (free_pages_.size() == 0)
            {
                page_container_.emplace_back(new PageType);
                Index::IndexOfPage indexOfPage = page_container_.size() - 1;
                free_pages_.insert(indexOfPage);
                return indexOfPage;
            }

            Index::IndexOfPage indexOfPage = *free_pages_.begin();
            return indexOfPage;
        }

    public:
        void remove_component_of_entity(EntityId entity)
        {
            Index::IndexInfo indexinfo = entity_to_component_.get_entity_indexinfo(entity);
            entity_to_component_.remove_entity_indexinfo(entity);
            PageTypeRef& page = page_container_[indexinfo.index_of_page];
            page->remove_component_at_index(indexinfo.component_pageindex);

            free_pages_.insert(indexinfo.index_of_page);
        }

        void remove_component_of_entity_v(EntityId entity) override
        {
            return remove_component_of_entity(entity);
        }

    public:
        template <typename... Args>
        ComponentType& add_component_to_entity(EntityId entity, Args&&... args)
        {
            LECS_ASSERT(has_this_component(entity) == false)
            
            Index::IndexOfPage indexOfFreePage = get_free_page_index_or_create_it();
            PageTypeRef& page = page_container_[indexOfFreePage];
            auto [pageIndexOfComponent, component] = page->add_component(entity, std::forward<Args>(args)...);
            entity_to_component_.add_entity_indexinfo(entity, Index::IndexInfo { .index_of_page = indexOfFreePage, .component_pageindex = pageIndexOfComponent });

            if (page->can_add_component() == false)
                free_pages_.erase(indexOfFreePage);

            return component;
        }

        ComponentType& get_entity_componenttype(EntityId entity)
        {
            Index::IndexInfo indexinfo = entity_to_component_.get_entity_indexinfo(entity);
            LECS_ASSERT(indexinfo.index_of_page < page_container_.size(), "Entity doesn't have this component")
            PageTypeRef& page = page_container_[indexinfo.index_of_page];
            return page->get_component_at_index(indexinfo.component_pageindex);
        }
        const ComponentType& get_entity_componenttype(EntityId entity) const
        {
            Index::IndexInfo indexinfo = entity_to_component_.get_entity_indexinfo(entity);
            LECS_ASSERT(indexinfo.index_of_page < page_container_.size(), "Entity doesn't have this component")
            const PageTypeRef& page = page_container_[indexinfo.index_of_page];
            return page->get_component_at_index(indexinfo.component_pageindex);
        }

        ComponentType* get_entity_componenttype_ptr(EntityId entity)
        {
            Index::IndexInfo indexinfo = entity_to_component_.get_entity_indexinfo(entity);
            if (indexinfo.index_of_page >= page_container_.size())
                return nullptr;
            PageTypeRef& page = page_container_[indexinfo.index_of_page];
            return page->get_component_at_indexptr(indexinfo.component_pageindex);
        }
        const ComponentType* get_entity_componenttype_ptr(EntityId entity) const
        {
            Index::IndexInfo indexinfo = entity_to_component_.get_entity_indexinfo(entity);
            if (indexinfo.index_of_page >= page_container_.size())
                return nullptr;
            const PageTypeRef& page = page_container_[indexinfo.index_of_page];
            return page->get_component_at_indexptr(indexinfo.component_pageindex);
        }

        const void* get_entity_componenttype_aliasedptr_v(EntityId entity) const override
        {
            return reinterpret_cast<const void*>(get_entity_componenttype_ptr(entity));
        }
        void* get_entity_componenttype_aliasedptr_v(EntityId entity) override
        {
            return reinterpret_cast<void*>(get_entity_componenttype_ptr(entity));
        }

    private:
        // Function = std::function<void(EntityId, ComponentType&)>
        template <typename Function, typename ComponentConstness>
        void foreach_storage_impl(Function&& function)
        requires (ComponentStorageInfo<ComponentType>::SEND_ENTITIES_POOL_ON_EACH == false);

    public:
        // Function = std::function<void(EntityId, ComponentType&)>
        template <typename Function>
        inline void foreach_storage(Function&& function)
        requires (ComponentStorageInfo<ComponentType>::SEND_ENTITIES_POOL_ON_EACH == false)
        {
            return foreach_storage_impl<Function, ComponentType>(std::forward<Function>(function));
        }
        
        // Function = std::function<void(EntityId, const ComponentType&)>
        template <typename Function>
        inline void foreach_storage(Function&& function) const
        requires (ComponentStorageInfo<ComponentType>::SEND_ENTITIES_POOL_ON_EACH == false)
        {
            return const_cast<CompressedComponentStorage<ComponentType>*>(this)->template foreach_storage_impl<Function, const ComponentType>(std::forward<Function>(function));
        }

    public:
        decltype(auto) entities_iterator_begin() const;
        decltype(auto) entities_iterator_end() const;
    };
}

#include "ccs_foreach-inl.h"
#include "ccs_iterator-inl.h"
