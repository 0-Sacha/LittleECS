#pragma once

#include "lite_entity.hxx"

namespace lecs {
    inline LiteEntity::LiteEntity() : registry_(nullptr), entityid_(EntityId::INVALID) {}

    inline LiteEntity::LiteEntity(Registry* registry, EntityId entityid) : registry_(registry), entityid_(entityid) {
        LECS_ASSERT(entityid_ != EntityId::INVALID)
        LECS_ASSERT(registry_ != nullptr)
        LECS_ASSERT(registry_->registry_has(entityid_))
    }

    template <typename ComponentType>
    bool LiteEntity::has() const {
        LECS_ASSERT(is_valid())

        return registry_->has<ComponentType>(entityid_);
    }

    template <typename ComponentType>
    const ComponentType& LiteEntity::get() const {
        LECS_ASSERT(is_valid())

        return registry_->get<ComponentType>(entityid_);
    }

    template <typename ComponentType>
    ComponentType& LiteEntity::get() {
        LECS_ASSERT(is_valid())

        return registry_->get<ComponentType>(entityid_);
    }

    template <typename... ComponentTypes>
    std::tuple<const ComponentTypes&...> LiteEntity::get_all() const {
        LECS_ASSERT(is_valid())

        return registry_->get_all<ComponentTypes...>(entityid_);
    }

    template <typename... ComponentTypes>
    std::tuple<ComponentTypes&...> LiteEntity::get_all() {
        LECS_ASSERT(is_valid())

        return registry_->get_all<ComponentTypes...>(entityid_);
    }

    template <typename ComponentType, typename... Args>
    ComponentType& LiteEntity::add(Args&&... args) {
        LECS_ASSERT(is_valid())

        return registry_->add<ComponentType>(entityid_, std::forward<Args>(args)...);
    }
}  // namespace lecs
