#pragma once

#include "lecs/core/core.hxx"

#include "componentid.hxx"

#include <string_view>
#include <typeinfo>

#ifdef LECS_COMPILER_MSVC
#define LECS_FUNCTION_SIGNATURE_ID __FUNCSIG__
#define LECS_FUNCTION_SIGNATURE_ID_PREFIX '<'
#define LECS_FUNCTION_SIGNATURE_ID_SUFFIX '>'
#else
#define LECS_FUNCTION_SIGNATURE_ID __PRETTY_FUNCTION__
#define LECS_FUNCTION_SIGNATURE_ID_PREFIX '='
#define LECS_FUNCTION_SIGNATURE_ID_SUFFIX ']'
#endif

namespace lecs::detail {
    class CompilerComponentIdGenerator {
    public:
        template <typename T>
        static constexpr ComponentId get_typeid() {
            return typeid(T).hash_code();
        }
    };

    class GlobalComponentIdGenerator {
    public:
        template <typename T>
        static ComponentId get_typeid() {
            static ComponentId id = ComponentId::INVALID;
            if (id != ComponentId::INVALID) return id;
            return id = next();
        }

    private:
        static ComponentId next() {
            ComponentId res = next_global_componentId_;
            next_global_componentId_.id_++;
            return res;
        }

    private:
        static inline ComponentId next_global_componentId_{ComponentId::FIRST};
    };

    class SignatureComponentIdGenerator {
    public:
        template <typename T>
        static constexpr ComponentId get_typeid() {
            std::string_view pretty_function{LECS_FUNCTION_SIGNATURE_ID};
            auto             first = pretty_function.find_first_not_of(' ', pretty_function.find_first_of(LECS_FUNCTION_SIGNATURE_ID_PREFIX) + 1);
            auto             value = pretty_function.substr(first, pretty_function.find_last_of(LECS_FUNCTION_SIGNATURE_ID_SUFFIX) - first);
            return std::hash<std::string_view>{}(value);
        }
    };
}  // namespace lecs::detail
