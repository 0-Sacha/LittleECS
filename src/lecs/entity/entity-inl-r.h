#pragma once

#include "entity.h"

namespace lecs {
    inline ConstEntity::ConstEntity() : registry_(nullptr), entityid_(EntityId::INVALID), components_container() {}

    inline ConstEntity::ConstEntity(const Registry* registry, EntityId entityId) : registry_(registry), entityid_(entityId), components_container() {
        LECS_ASSERT(entityid_ != EntityId::INVALID)
        LECS_ASSERT(registry_ != nullptr)
        LECS_ASSERT(registry_->registry_has(entityid_))

        refresh();
    }

    inline void ConstEntity::refresh() {
        LECS_ASSERT(is_valid())

        for (auto& container : registry_->get_componentid_to_componentdata()) {
            const void* component = container.second.component_storage->get_entity_componenttype_aliasedptr_v(entityid_);
            if (component != nullptr) {
                components_container.insert({container.first, reinterpret_cast<const void*>(component)});
            }
        }
    }

    template <typename ComponentType>
    const ComponentType* ConstEntity::GetComponentPtr() const {
        LECS_ASSERT(is_valid())

        ComponentId componentId = Registry::ComponentIdGenerator::get_typeid<ComponentType>();

        const auto component = components_container.find(componentId);

        if (component == components_container.end()) return nullptr;

        return reinterpret_cast<const ComponentType*>(component->second);
    }

    template <typename ComponentType>
    bool ConstEntity::has() const {
        LECS_ASSERT(is_valid())

        if constexpr (detail::ComponentStorageInfo<ComponentType>::PTR_TO_COMPONENT_VALID) {
            return GetComponentPtr<ComponentType>() != nullptr;
        } else {
            return registry_->has<ComponentType>(entityid_);
        }
    }

    template <typename ComponentType>
    const ComponentType& ConstEntity::get() const {
        LECS_ASSERT(is_valid())

        if constexpr (detail::ComponentStorageInfo<ComponentType>::PTR_TO_COMPONENT_VALID) {
            return *GetComponentPtr<ComponentType>();
        } else {
            return registry_->get<ComponentType>(entityid_);
        }
    }
    template <typename ComponentType>
    const ComponentType* ConstEntity::GetPtr() const {
        LECS_ASSERT(is_valid())

        if constexpr (detail::ComponentStorageInfo<ComponentType>::PTR_TO_COMPONENT_VALID) {
            return GetComponentPtr<ComponentType>();
        } else {
            return registry_->GetPtr<ComponentType>(entityid_);
        }
    }
    template <typename... ComponentTypes>
    std::tuple<const ComponentTypes&...> ConstEntity::get_all() const {
        LECS_ASSERT(is_valid())

        return std::tuple<const ComponentTypes&...>(get<ComponentTypes>(entityid_)...);
    }
}  // namespace lecs

namespace lecs {
    inline Entity::Entity() : ConstEntity() {}

    inline Entity::Entity(Registry* registry, EntityId entityId) : ConstEntity(registry, entityId) {}

    template <typename... ComponentTypes>
    std::tuple<ComponentTypes&...> Entity::get_all() {
        LECS_ASSERT(is_valid())

        return std::tuple<ComponentTypes&...>(get<ComponentTypes>(entityid_)...);
    }

    template <typename ComponentType, typename... Args>
    ComponentType& Entity::Add(Args&&... args) {
        LECS_ASSERT(is_valid())

        ComponentId componentId = Registry::ComponentIdGenerator::get_typeid<ComponentType>();

        ComponentType& res = registry_->Add<ComponentType>(entityid_, std::forward<Args>(args)...);
        components_container.insert({componentId, &res});
        return res;
    }
}  // namespace lecs
