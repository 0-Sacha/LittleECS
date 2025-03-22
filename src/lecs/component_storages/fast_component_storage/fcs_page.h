#pragma once

#include "lecs/registry/component_storage.h"

#include <array>

namespace lecs::detail {
    template <typename ComponentType, std::size_t PAGE_SIZE>
        requires(PAGE_SIZE % sizeof(std::size_t) == 0)
    class FastComponentStoragePage {
    private:
        struct ComponentDataBuffer {
            union DataStorageType {
                std::uint8_t  storage_data[sizeof(ComponentType)];
                ComponentType component_value;

                DataStorageType() {}
                ~DataStorageType() {}
            };
            DataStorageType data{};
        };

    public:
        struct EntityLinkedWithoutRef {
            typename EntityId::Type Entity;

            inline bool constexpr is_valid() const {
                return Entity != EntityId::INVALID;
            }

            inline void constexpr set_invalid() {
                Entity = EntityId::INVALID;
            }
        };

        struct EntityLinkedWithRef {
            typename EntityId::Type    Entity;
            Index::index_in_alive_list index_in_alive_list;

            inline bool constexpr is_valid() const {
                return Entity != EntityId::INVALID;
            }

            inline void constexpr set_invalid() {
                Entity = EntityId::INVALID;
            }
        };

        using EntityLinked = std::conditional_t<ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF, EntityLinkedWithRef, EntityLinkedWithoutRef>;

    private:
        template <typename... Args>
        inline ComponentType& construct_at(Index::ComponentPageIndex index, Args&&... args) {
            ComponentDataBuffer* buffer    = &page_[index];
            ComponentType*       component = new (buffer) ComponentType(std::forward<Args>(args)...);
            return *component;
        }

        inline void destroy_at(Index::ComponentPageIndex index) {
            ComponentType& component = page_[index].data.component_value;
            component.~ComponentType();
        }

    public:
        FastComponentStoragePage() {
            for (std::size_t i = 0; i < PAGE_SIZE; ++i)
                entities_linked_[i].set_invalid();
        }

        ~FastComponentStoragePage() {
            // ForEach([this](Index::ComponentPageIndex index) { this->destroy_at(index); });
        }

    protected:
        std::array<ComponentDataBuffer, PAGE_SIZE> page_;
        EntityLinked                               entities_linked_[PAGE_SIZE];

    public:
        inline typename EntityId::Type get_entityid_at_index(Index::ComponentPageIndex index) const {
            return entities_linked_[index].Entity;
        }

        inline bool has_entity_at_index(Index::ComponentPageIndex index) const {
            return entities_linked_[index].is_valid();
        }

        inline Index::index_in_alive_list get_index_in_alive_list_at_index(Index::ComponentPageIndex index) const {
            if constexpr (ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF)
                return entities_linked_[index].index_in_alive_list;
            else
                return 0;
        }

    public:
        inline void remove_component_at_index(Index::ComponentPageIndex index) {
            LECS_ASSERT(has_entity_at_index(index) == true, "There are no component linked to this entity at this page")
            destroy_at(index);
            entities_linked_[index].set_invalid();
        }

    public:
        template <typename... Args>
            requires(ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF == true)
        ComponentType& add_component(EntityId entity, Index::ComponentPageIndex index, Index::index_in_alive_list index_in_alive_list, Args&&... args) {
            LECS_ASSERT(has_entity_at_index(index) == false, "Can't add this entity to this because it has the same id as another one")

            ComponentType& component = construct_at(index, std::forward<Args>(args)...);

            entities_linked_[index] = EntityLinked{.Entity = entity, .index_in_alive_list = index_in_alive_list};

            return component;
        }

        template <typename... Args>
            requires(ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF == false)
        ComponentType& add_component(EntityId entity, Index::ComponentPageIndex index, Args&&... args) {
            LECS_ASSERT(has_entity_at_index(index) == false, "Can't add this entity to this because it has the same id as another one")

            ComponentType& component = construct_at(index, std::forward<Args>(args)...);

            entities_linked_[index] = EntityLinked{.Entity = entity};

            return component;
        }

        ComponentType& get_component_at_index(Index::ComponentPageIndex index) {
            LECS_ASSERT(index < page_.size(), "There are no component linked to this entity at this page")
            LECS_ASSERT(has_entity_at_index(index) == true, "There are no component linked to this entity at this page")

            return *reinterpret_cast<ComponentType*>(&page_[index]);
        }

        const ComponentType& get_component_at_index(Index::ComponentPageIndex index) const {
            LECS_ASSERT(index < page_.size(), "There are no component linked to this entity at this page")
            LECS_ASSERT(has_entity_at_index(index) == true, "There are no component linked to this entity at this page")

            return *reinterpret_cast<ComponentType*>(&page_[index]);
        }

        ComponentType* get_component_at_indexptr(Index::ComponentPageIndex index) {
            if (index >= page_.size()) return nullptr;
            if (has_entity_at_index(index) == false) return nullptr;
            return reinterpret_cast<ComponentType*>(&page_[index]);
        }

        const ComponentType* get_component_at_indexptr(Index::ComponentPageIndex index) const {
            if (index >= page_.size()) return nullptr;
            if (has_entity_at_index(index) == false) return nullptr;
            return reinterpret_cast<const ComponentType*>(&page_[index]);
        }
    };
}  // namespace lecs::detail
