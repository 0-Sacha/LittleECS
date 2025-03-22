#pragma once

#include "lecs/detail/componentid.hxx"
#include "lecs/detail/entityid.hxx"

#include "lecs/detail/componentid_generator.hxx"

#include <any>

namespace lecs::detail {
    namespace Index {
        using GlobalIndexOfComponent = std::size_t;
        using ComponentPageIndex     = std::size_t;
        using IndexOfPage            = std::size_t;

        using index_in_alive_list = std::size_t;

        struct IndexInfo;
    };  // namespace Index

    struct Index::IndexInfo {
        Index::IndexOfPage        index_of_page;
        Index::ComponentPageIndex component_pageindex;

        [[nodiscard]] inline constexpr bool is_valid() {
            return index_of_page != std::numeric_limits<std::size_t>::max();
        }

        inline constexpr void set_invalid() {
            index_of_page = std::numeric_limits<std::size_t>::max();
        }
    };

    class IComponentStorage {
    public:
        virtual ~IComponentStorage() = default;

    public:
        virtual bool        has_this_component_v(EntityId entity) const                  = 0;
        virtual void        remove_component_of_entity_v(EntityId entity)                = 0;
        virtual const void* get_entity_componenttype_aliasedptr_v(EntityId entity) const = 0;
        virtual void*       get_entity_componenttype_aliasedptr_v(EntityId entity)       = 0;
    };

    template <typename ComponentType>
    struct TypeValidForComponentStorage {
        static constexpr bool value = std::is_void_v<ComponentType> == false;
    };

    template <typename ComponentType>
        requires(TypeValidForComponentStorage<ComponentType>::value)
    class FastComponentStorage;

    template <typename ComponentType>
        requires(TypeValidForComponentStorage<ComponentType>::value)
    class CompressedComponentStorage;

    template <typename ComponentType>
    struct DefaultComponentStorageInfo {
        struct FastComponent {
            using StorageType                                                         = FastComponentStorage<ComponentType>;
            static constexpr Index::GlobalIndexOfComponent PAGE_SIZE                  = 1'024;
            static constexpr bool                          HAS_ENTITIES_REF           = true;
            static constexpr bool                          USE_MAP_VERSION            = false;
            static constexpr bool                          SEND_ENTITIES_POOL_ON_EACH = false;

            static constexpr bool PTR_TO_COMPONENT_VALID = true;
        };

        struct FastComponentWithoutREF {
            using StorageType                                                         = FastComponentStorage<ComponentType>;
            static constexpr Index::GlobalIndexOfComponent PAGE_SIZE                  = 1'024;
            static constexpr bool                          HAS_ENTITIES_REF           = false;
            static constexpr bool                          USE_MAP_VERSION            = false;
            static constexpr bool                          SEND_ENTITIES_POOL_ON_EACH = true;

            static constexpr bool PTR_TO_COMPONENT_VALID = true;
        };

        struct RareComponent {
            using StorageType                                                         = CompressedComponentStorage<ComponentType>;
            static constexpr Index::GlobalIndexOfComponent PAGE_SIZE                  = 2'048;
            static constexpr bool                          HAS_ENTITIES_REF           = true;
            static constexpr bool                          USE_MAP_VERSION            = false;
            static constexpr bool                          SEND_ENTITIES_POOL_ON_EACH = false;

            static constexpr bool PTR_TO_COMPONENT_VALID = true;
        };

        struct CommonComponent {
            using StorageType                                                         = CompressedComponentStorage<ComponentType>;
            static constexpr Index::GlobalIndexOfComponent PAGE_SIZE                  = 4'096;
            static constexpr bool                          HAS_ENTITIES_REF           = false;
            static constexpr bool                          USE_MAP_VERSION            = false;
            static constexpr bool                          SEND_ENTITIES_POOL_ON_EACH = false;

            static constexpr bool PTR_TO_COMPONENT_VALID = true;
        };

        using Default = FastComponent;
    };

    template <typename ComponentType>
    struct ComponentStorageInfo : public DefaultComponentStorageInfo<ComponentType>::Default {};

}  // namespace lecs::detail
