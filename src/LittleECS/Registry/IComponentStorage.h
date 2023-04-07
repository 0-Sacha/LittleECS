#pragma once

#include "LittleECS/Detail/ComponentId.h"
#include "LittleECS/Detail/EntityId.h"

#include "LittleECS/Detail/ComponentIdGenerator.h"

#include <any>

namespace LittleECS::Detail
{
    namespace Index
    {
        using GlobalIndexOfComponent = std::size_t;
        using PageIndexOfComponent = std::size_t;
        using IndexOfPage = std::size_t;

        using IndexInAliveList = std::size_t;

        struct IndexInfo
        {
            IndexOfPage IndexOfPage;
            PageIndexOfComponent PageIndexOfComponent;

            [[nodiscard]] inline constexpr bool IsValid()
            {
                return IndexOfPage != std::numeric_limits<std::size_t>::max();
            }

            inline constexpr void SetInvalid()
            {
                IndexOfPage = std::numeric_limits<std::size_t>::max();
            }
        };

        [[nodiscard]] inline constexpr bool IsPowerOfTwo(const std::size_t value) noexcept
        {
            return value && ((value & (value - 1)) == 0);
        }
    };

    class IComponentStorage
    {
    public:
        virtual ~IComponentStorage() = default;

	public:
		virtual bool EntityHasThisComponent(EntityId entity) const = 0;
		virtual void RemoveComponentOfEntity(EntityId entity) = 0;
	};


    template <typename ComponentType>
    class FastComponentStorage;

    template <typename ComponentType>
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
        };

        struct FastComponentWithoutREF
        {
            using StorageType = FastComponentStorage<ComponentType>;
    		static constexpr Index::GlobalIndexOfComponent PAGE_SIZE = 1024;
            static constexpr bool HAS_ENTITIES_REF = false;
            static constexpr bool USE_MAP_VERSION = false;
            static constexpr bool SEND_ENTITIES_POOL_ON_EACH = true;
        };

        struct RareComponent
        {
            using StorageType = CompressedComponentStorage<ComponentType>;
            static constexpr Index::GlobalIndexOfComponent PAGE_SIZE = 2048;
            static constexpr bool HAS_ENTITIES_REF = true;
            static constexpr bool USE_MAP_VERSION = false;
            static constexpr bool SEND_ENTITIES_POOL_ON_EACH = false;
        };

        struct CommonComponent
        {
            using StorageType = CompressedComponentStorage<ComponentType>;
            static constexpr Index::GlobalIndexOfComponent PAGE_SIZE = 4096;
            static constexpr bool HAS_ENTITIES_REF = false;
            static constexpr bool USE_MAP_VERSION = false;
            static constexpr bool SEND_ENTITIES_POOL_ON_EACH = false;
        };

        using Default = CommonComponent;
    };

	template <typename ComponentType>
    struct ComponentStorageInfo : public DefaultComponentStorageInfo<ComponentType>::Default{};

}
