#pragma once

#include "lecs/core/core.hxx"

#include <limits>

namespace lecs {
    class EntityId {
    public:
        using Type                    = std::size_t;
        static constexpr Type INVALID = std::numeric_limits<Type>::max();
        static constexpr Type FIRST   = 0;

    public:
        inline constexpr EntityId(Type id = FIRST) : id_(id) {}

        inline constexpr operator Type() const {
            return id_;
        }

        inline constexpr EntityId& operator=(Type id) {
            id_ = id;
            return *this;
        }

    public:
        Type id_;
    };

    inline bool constexpr operator==(EntityId lhs, EntityId rhs) {
        return lhs.id_ == rhs.id_;
    }

    inline bool constexpr operator==(EntityId lhs, EntityId::Type rhs) {
        return lhs.id_ == rhs;
    }

    inline bool constexpr operator==(EntityId::Type lhs, EntityId rhs) {
        return lhs == rhs.id_;
    }
}  // namespace lecs
