#pragma once

#include "LittleECS/Detail/ComponentId.h"
#include "LittleECS/Detail/EntityId.h"

#include "LittleECS/Detail/ComponentIdGenerator.h"

#include <any>

namespace LECS::Detail
{
    namespace Index
    {
        using GlobalIndexOfComponent = std::size_t;
        using PageIndexOfComponent = std::size_t;
        using IndexOfPage = std::size_t;

        using IndexInAliveList = std::size_t;

        struct IndexInfo;

        [[nodiscard]] inline constexpr bool IsPowerOfTwo(const std::size_t value) noexcept
        {
            return value && ((value & (value - 1)) == 0);
        }
    };

    struct Index::IndexInfo
    {
        Index::IndexOfPage IndexOfPage;
        Index::PageIndexOfComponent PageIndexOfComponent;

        [[nodiscard]] inline constexpr bool IsValid()
        {
            return IndexOfPage != std::numeric_limits<std::size_t>::max();
        }

        inline constexpr void SetInvalid()
        {
            IndexOfPage = std::numeric_limits<std::size_t>::max();
        }
    };

    class IComponentStorage
    {
    public:
        virtual ~IComponentStorage() = default;

    public:
        virtual bool HasThisComponentV(EntityId entity) const = 0;
        virtual void RemoveComponentOfEntityV(EntityId entity) = 0;
        virtual const void* GetComponentAliasedPtrV(EntityId entity) const = 0;
        virtual void* GetComponentAliasedPtrV(EntityId entity) = 0;
    };

    template <typename ComponentType>
    struct TypeValidForComponentStorage
    {
        static constexpr bool Value = std::is_void_v<ComponentType> == false;
    };

    template <typename ComponentType>
    requires (TypeValidForComponentStorage<ComponentType>::Value)
    class FastComponentStorage;

    template <typename ComponentType>
    requires (TypeValidForComponentStorage<ComponentType>::Value)
    class CompressedComponentStorage;


    template <typename ComponentType>
    struct DefaultComponentStorageInfo
    {
        struct FastComponent
        {
            using StorageType = FastComponentStorage<ComponentType>;
            static constexpr Index::GlobalIndexOfComponent PAGE_SIZE = 1024;
            static constexpr bool HAS_ENTITIES_REF = true;
            static constexpr bool USE_MAP_VERSION = false;
            static constexpr bool SEND_ENTITIES_POOL_ON_EACH = false;

            static constexpr bool PTR_TO_COMPONENT_VALID = true;
        };

        struct FastComponentWithoutREF
        {
            using StorageType = FastComponentStorage<ComponentType>;
            static constexpr Index::GlobalIndexOfComponent PAGE_SIZE = 1024;
            static constexpr bool HAS_ENTITIES_REF = false;
            static constexpr bool USE_MAP_VERSION = false;
            static constexpr bool SEND_ENTITIES_POOL_ON_EACH = true;

            static constexpr bool PTR_TO_COMPONENT_VALID = true;
        };

        struct RareComponent
        {
            using StorageType = CompressedComponentStorage<ComponentType>;
            static constexpr Index::GlobalIndexOfComponent PAGE_SIZE = 2048;
            static constexpr bool HAS_ENTITIES_REF = true;
            static constexpr bool USE_MAP_VERSION = false;
            static constexpr bool SEND_ENTITIES_POOL_ON_EACH = false;

            static constexpr bool PTR_TO_COMPONENT_VALID = true;
        };

        struct CommonComponent
        {
            using StorageType = CompressedComponentStorage<ComponentType>;
            static constexpr Index::GlobalIndexOfComponent PAGE_SIZE = 4096;
            static constexpr bool HAS_ENTITIES_REF = false;
            static constexpr bool USE_MAP_VERSION = false;
            static constexpr bool SEND_ENTITIES_POOL_ON_EACH = false;

            static constexpr bool PTR_TO_COMPONENT_VALID = true;
        };

        using Default = FastComponent;
    };

    template <typename ComponentType>
    struct ComponentStorageInfo : public DefaultComponentStorageInfo<ComponentType>::Default{};

}
