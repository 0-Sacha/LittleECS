#pragma once

#include "entityid.hxx"

#include <queue>
#include <set>

namespace lecs::detail {

    class EntityIdGenerator {
    public:
        EntityId get_new_entityid() {
            typename EntityId::Type res;

            if (free_entities_id_.size() > 0)
                res = free_entities_id_.extract(free_entities_id_.begin()).value();
            else
                res = current_index_++;

            LECS_ASSERT(alive_entities_id_.contains(res) == false, "Create an EntityId with an already assigned id")
            alive_entities_id_.insert(res);
            return res;
        }

        void entityid_delete(EntityId entity) {
            typename EntityId::Type id = entity.id_;
            if (id == current_index_ - 1) {
                current_index_--;
                return;
            }

            LECS_ASSERT(alive_entities_id_.contains(id) == true, "Destroy an EntityId with an non assigned id")

            alive_entities_id_.erase(id);
            free_entities_id_.insert(id);
        }

        const std::set<typename EntityId::Type>& get_alives_entities() const {
            return alive_entities_id_;
        }

        bool has_entityid(EntityId entity) const {
            return alive_entities_id_.contains(entity);
        }

    protected:
        typename EntityId::Type           current_index_ = EntityId::FIRST;
        std::set<typename EntityId::Type> free_entities_id_;
        std::set<typename EntityId::Type> alive_entities_id_;
    };

}  // namespace lecs::detail
