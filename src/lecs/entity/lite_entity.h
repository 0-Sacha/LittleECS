#pragma once

#include "lecs/detail/componentid.h"
#include "lecs/detail/entityid.h"

namespace lecs {
    class Registry;
    class LiteEntity {
    public:
        LiteEntity();
        LiteEntity(Registry* registry, EntityId entityid);

    protected:
        Registry* registry_;
        EntityId  entityid_;

    public:
        inline EntityId GetEntityId() {
            return entityid_;
        }
        inline operator EntityId() {
            return entityid_;
        }
        inline operator bool() {
            return entityid_ != EntityId::INVALID;
        }

    public:
        bool is_valid() const {
            return entityid_ != EntityId::INVALID && registry_ != nullptr;
        }

        void Invalidate() {
            entityid_ = EntityId::INVALID;
            registry_ = nullptr;
        }

    public:
        template <typename ComponentType>
        bool has() const;
        template <typename ComponentType>
        const ComponentType& get() const;
        template <typename ComponentType>
        ComponentType& get();
        template <typename... ComponentTypes>
        std::tuple<const ComponentTypes&...> get_all() const;
        template <typename... ComponentTypes>
        std::tuple<ComponentTypes&...> get_all();
        template <typename ComponentType, typename... Args>
        ComponentType& add(Args&&... args);
    };
}  // namespace lecs