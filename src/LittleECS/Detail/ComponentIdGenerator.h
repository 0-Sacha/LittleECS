#pragma once

#include "LittleECS/Core/Core.h"

#include "ComponentId.h"

#include <string_view>

#ifdef LECS_COMPILER_MSVC
    #define LECS_FUNCTION_SIGNATURE_ID __FUNCSIG__
    #define LECS_FUNCTION_SIGNATURE_ID_PREFIX '<'
    #define LECS_FUNCTION_SIGNATURE_ID_SUFFIX '>'
#else
    #define LECS_FUNCTION_SIGNATURE_ID __PRETTY_FUNCTION__
    #define LECS_FUNCTION_SIGNATURE_ID_PREFIX '='
    #define LECS_FUNCTION_SIGNATURE_ID_SUFFIX ']'
#endif

namespace LECS::Detail
{
    class CompilerComponentIdGenerator
    {
    public:
        template <typename T>
        static constexpr ComponentId GetTypeId()
        {
            return typeid(T).hash_code();
        }
    };

    class GlobalComponentIdGenerator
    {
    public:
        template <typename T>
        static ComponentId GetTypeId()
        {
            static ComponentId id = ComponentId::INVALID;
            if (id != ComponentId::INVALID)
                return id;
            return id = Next();
        }

    private:
        static ComponentId Next()
        {
            ComponentId res = m_NextGlobalComponentId;
            m_NextGlobalComponentId.Id++;
            return res;
        }

    private:
        static inline ComponentId m_NextGlobalComponentId{ ComponentId::FIRST };
    };

    class SignatureComponentIdGenerator
    {
    public:
        template <typename T>
        static constexpr ComponentId GetTypeId()
        {
            std::string_view pretty_function{ LECS_FUNCTION_SIGNATURE_ID };
            auto first = pretty_function.find_first_not_of(' ', pretty_function.find_first_of(LECS_FUNCTION_SIGNATURE_ID_PREFIX) + 1);
            auto value = pretty_function.substr(first, pretty_function.find_last_of(LECS_FUNCTION_SIGNATURE_ID_SUFFIX) - first);
            return std::hash<std::string_view>{}(value);
        }
    };
}
