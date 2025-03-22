#pragma once

#include "lecs/registry/component_storage.h"
#include "lecs/detail/exception.h"

#include "fcs_page.h"

#include <memory>
#include <any>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace lecs::detail {
    template <typename ComponentType>
        requires(TypeValidForComponentStorage<ComponentType>::value)
    class FastComponentStorage : public IComponentStorage {
    public:
        using M_Type = FastComponentStorage<ComponentType>;

        static constexpr Index::GlobalIndexOfComponent PAGE_SIZE = ComponentStorageInfo<ComponentType>::PAGE_SIZE;

        using PageType                   = FastComponentStoragePage<ComponentType, PAGE_SIZE>;
        using PageTypeRef                = std::unique_ptr<PageType>;
        using PagesContainer             = std::vector<PageTypeRef>;
        using AliveEntitiesContainerType = std::vector<typename EntityId::Type>;
        using AliveEntitiesContainer     = std::conditional_t<ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF, AliveEntitiesContainerType, int>;

    public:
        FastComponentStorage() : page_container_(), alive_entities_container_() {
            if constexpr (ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF) alive_entities_container_.reserve(PAGE_SIZE);
        }

        ~FastComponentStorage() override {}

    protected:
        PagesContainer         page_container_;
        AliveEntitiesContainer alive_entities_container_;

    protected:
        inline Index::IndexInfo get_entity_indexinfo(EntityId entity) const {
            Index::IndexInfo indexinfo;
            indexinfo.index_of_page       = entity.id_ / PAGE_SIZE;
            indexinfo.component_pageindex = entity.id_ % PAGE_SIZE;
            return indexinfo;
        }

    public:
        bool has_this_component(EntityId entity) const {
            Index::IndexInfo indexinfo = get_entity_indexinfo(entity);

            if (indexinfo.index_of_page >= page_container_.size()) return false;
            if (page_container_[indexinfo.index_of_page] == nullptr) return false;

            return page_container_[indexinfo.index_of_page]->has_entity_at_index(indexinfo.component_pageindex);
        }

        bool has_this_component_v(EntityId entity) const override {
            return has_this_component(entity);
        }

        void remove_component_of_entity(EntityId entity) {
            Index::IndexInfo indexinfo = get_entity_indexinfo(entity);
            PageTypeRef&     page      = page_container_[indexinfo.index_of_page];

            if constexpr (ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF) {
                Index::index_in_alive_list index_in_alive_list = page->get_index_in_alive_list_at_index(indexinfo.component_pageindex);
                typename EntityId::Type    last_entity         = alive_entities_container_.back();
                alive_entities_container_[index_in_alive_list] = last_entity;
                alive_entities_container_.pop_back();
            }

            page->remove_component_at_index(indexinfo.component_pageindex);
        }

        void remove_component_of_entity_v(EntityId entity) override {
            return remove_component_of_entity(entity);
        }

    public:
        template <typename... Args>
        ComponentType& add_component_to_entity(EntityId entity, Args&&... args) {
            Index::IndexInfo indexinfo = get_entity_indexinfo(entity);

            if (indexinfo.index_of_page >= page_container_.size()) {
                page_container_.resize(indexinfo.index_of_page + 1);
            }

            PageTypeRef& page = page_container_[indexinfo.index_of_page];

            if (page == nullptr) {
                page = std::make_unique<PageType>();
            }

            if constexpr (ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF) {
                alive_entities_container_.emplace_back(entity.id_);
                return page->add_component(entity, indexinfo.component_pageindex, alive_entities_container_.size() - 1, std::forward<Args>(args)...);
            } else
                return page->add_component(entity, indexinfo.component_pageindex, std::forward<Args>(args)...);
        }

        ComponentType& get_entity_componenttype(EntityId entity) {
            Index::IndexInfo indexinfo = get_entity_indexinfo(entity);
            LECS_ASSERT(indexinfo.index_of_page < page_container_.size(), "Entity doesn't have this component")
            PageTypeRef& page = page_container_[indexinfo.index_of_page];
            return page->get_component_at_index(indexinfo.component_pageindex);
        }
        const ComponentType& get_entity_componenttype(EntityId entity) const {
            Index::IndexInfo indexinfo = get_entity_indexinfo(entity);
            LECS_ASSERT(indexinfo.index_of_page < page_container_.size(), "Entity doesn't have this component")
            const PageTypeRef& page = page_container_[indexinfo.index_of_page];
            return page->get_component_at_index(indexinfo.component_pageindex);
        }

        ComponentType* get_entity_componenttype_ptr(EntityId entity) {
            Index::IndexInfo indexinfo = get_entity_indexinfo(entity);
            if (indexinfo.index_of_page >= page_container_.size()) return nullptr;
            PageTypeRef& page = page_container_[indexinfo.index_of_page];
            return page->get_component_at_indexptr(indexinfo.component_pageindex);
        }
        const ComponentType* get_entity_componenttype_ptr(EntityId entity) const {
            Index::IndexInfo indexinfo = get_entity_indexinfo(entity);
            if (indexinfo.index_of_page >= page_container_.size()) return nullptr;
            const PageTypeRef& page = page_container_[indexinfo.index_of_page];
            return page->get_component_at_indexptr(indexinfo.component_pageindex);
        }

        const void* get_entity_componenttype_aliasedptr_v(EntityId entity) const override {
            return reinterpret_cast<const void*>(get_entity_componenttype_ptr(entity));
        }
        void* get_entity_componenttype_aliasedptr_v(EntityId entity) override {
            return reinterpret_cast<void*>(get_entity_componenttype_ptr(entity));
        }

    public:
        // Function = std::function<void(EntityId, ComponentType&)>
        template <typename Function>
        void foreach_storage(Function&& function)
            requires(ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF && ComponentStorageInfo<ComponentType>::SEND_ENTITIES_POOL_ON_EACH == false);

        // Function = std::function<void(EntityId, const ComponentType&)>
        template <typename Function>
        void foreach_storage(Function&& function) const
            requires(ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF && ComponentStorageInfo<ComponentType>::SEND_ENTITIES_POOL_ON_EACH == false);

        // Function = std::function<void(EntityId, ComponentType&)>
        template <typename Function>
        void foreach_storage(Function&& function, const auto& alive_entities_registry)
            requires(ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF == false && ComponentStorageInfo<ComponentType>::SEND_ENTITIES_POOL_ON_EACH);

        // Function = std::function<void(EntityId, const ComponentType&)>
        template <typename Function>
        void foreach_storage(Function&& function, const auto& alive_entities_registry) const
            requires(ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF == false && ComponentStorageInfo<ComponentType>::SEND_ENTITIES_POOL_ON_EACH);

    public:
        decltype(auto) entities_iterator_begin() const
            requires(ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF && ComponentStorageInfo<ComponentType>::SEND_ENTITIES_POOL_ON_EACH == false);

        decltype(auto) entities_iterator_end() const
            requires(ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF && ComponentStorageInfo<ComponentType>::SEND_ENTITIES_POOL_ON_EACH == false);

        decltype(auto) entities_iterator_begin(const auto& alive_entities_registry) const
            requires(ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF == false && ComponentStorageInfo<ComponentType>::SEND_ENTITIES_POOL_ON_EACH);

        decltype(auto) entities_iterator_end(const auto& alive_entities_registry) const
            requires(ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF == false && ComponentStorageInfo<ComponentType>::SEND_ENTITIES_POOL_ON_EACH);
    };
}  // namespace lecs::detail

#include "fcs_each-inl.h"
#include "fcs_iterator-inl.h"
