#pragma once

#include "ccs_page.hxx"

#include <unordered_map>

namespace lecs::detail {
    class CCS_EntityToComponent_Map {
    public:
        using Container = std::unordered_map<typename EntityId::Type, Index::IndexInfo>;

    private:
        Container container_;

    public:
        const Container& get_container() {
            return container_;
        }

        inline bool has_entity(EntityId entity) const {
            return container_.contains(entity.id_);
        }

        inline Index::IndexInfo get_entity_indexinfo(EntityId entity) const {
            LECS_ASSERT(has_entity(entity) == true)
            return container_.at(entity.id_);
        }

        inline void add_entity_indexinfo(EntityId entity, Index::IndexInfo indexinfo) {
            LECS_ASSERT(has_entity(entity) == false)
            container_[entity.id_] = indexinfo;
        }

        inline void remove_entity_indexinfo(EntityId entity) {
            LECS_ASSERT(has_entity(entity) == true)
            container_[entity.id_].set_invalid();
        }
    };
}  // namespace lecs::detail
