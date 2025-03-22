#pragma once

#include "lecs/core/core.h"

#include <limits>

namespace lecs {
    struct ComponentId {
    public:
        using Type                    = std::size_t;
        static constexpr Type INVALID = std::numeric_limits<Type>::max();
        static constexpr Type FIRST   = 0;

    public:
        inline constexpr ComponentId(Type id = FIRST) : id_(id) {}

        inline constexpr operator Type() const {
            return id_;
        }

        inline constexpr ComponentId& operator=(Type id) {
            id_ = id;
            return *this;
        }

    public:
        Type id_;
    };

    inline bool constexpr operator==(ComponentId lhs, ComponentId rhs) {
        return lhs.id_ == rhs.id_;
    }

    inline bool constexpr operator==(ComponentId lhs, ComponentId::Type rhs) {
        return lhs.id_ == rhs;
    }

    inline bool constexpr operator==(ComponentId::Type lhs, ComponentId rhs) {
        return lhs == rhs.id_;
    }
}  // namespace lecs
