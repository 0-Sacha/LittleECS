#pragma once

#include "lite_entity.h"

namespace lecs
{
    inline LiteEntity::LiteEntity()
        : registry_(nullptr)
        , entityid_(EntityId::INVALID)
    {
    }

    inline LiteEntity::LiteEntity(Registry* registry, EntityId entityId)
        : registry_(registry)
        , entityid_(entityId)
    {
        LECS_ASSERT(entityid_ != EntityId::INVALID)
        LECS_ASSERT(registry_ != nullptr)
        LECS_ASSERT(registry_->registry_has(entityid_))
    }

    template <typename ComponentType>
    bool LiteEntity::has() const
    {
        LECS_ASSERT(is_valid())

        return registry_->has<ComponentType>(entityid_);
    }

    template <typename ComponentType>
    const ComponentType& LiteEntity::get() const
    {
        LECS_ASSERT(is_valid())
        
        return registry_->get<ComponentType>(entityid_);
    }

    template <typename ComponentType>
    ComponentType& LiteEntity::get()
    {
        LECS_ASSERT(is_valid())

        return registry_->get<ComponentType>(entityid_);
    }

    template <typename... ComponentTypes>
    std::tuple<const ComponentTypes&...> LiteEntity::get_all() const
    {
        LECS_ASSERT(is_valid())

        return registry_->get_all<ComponentTypes...>(entityid_);
    }

    template <typename... ComponentTypes>
    std::tuple<ComponentTypes&...> LiteEntity::get_all()
    {
        LECS_ASSERT(is_valid())
        
        return registry_->get_all<ComponentTypes...>(entityid_);
    }

    template <typename ComponentType, typename... Args>
    ComponentType& LiteEntity::Add(Args&&... args)
    {
        LECS_ASSERT(is_valid())
        
        return registry_->Add<ComponentType>(entityid_, std::forward<Args>(args)...);
    }
}
