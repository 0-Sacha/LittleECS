#pragma once

#include "lecs/detail/entityid_generator.h"

#include "component_storage.h"

#include "lecs/entity/entity.h"
#include "lecs/entity/lite_entity.h"

#include "views/basic_view.h"

#include <unordered_map>
#include <memory>

namespace lecs
{
    class Registry
    {
    public:
        struct ComponentData
        {
            std::unique_ptr<detail::IComponentStorage> component_storage;
            std::function<void(EntityId)> on_construct;
            std::function<void(EntityId)> on_destruct;
        };
        using ComponentIdToComponentData = std::unordered_map<ComponentId::Type, ComponentData>;
        using ComponentIdGenerator = detail::CompilerComponentIdGenerator;

    public:
        Registry() {}

    protected:
        ComponentIdToComponentData componentid_to_component_data_;
        detail::EntityIdGenerator entityid_generator_;

    // Entity Management
    public:
        const detail::EntityIdGenerator& get_entityid_generator() const
        {
            return entityid_generator_;
        }
        const ComponentIdToComponentData& get_componentid_to_componentdata() const
        {
            return componentid_to_component_data_;
        }

        bool registry_has(EntityId entity) const
        {
            return entityid_generator_.has_entityid(entity);
        }
        
        EntityId create_entityid()
        {
            return entityid_generator_.get_new_entityid();
        }

        void destroy_entityid(EntityId entity)
        {
            for (auto& container : componentid_to_component_data_)
            {
                if (container.second.component_storage->has_this_component_v(entity))
                {
                    container.second.component_storage->remove_component_of_entity_v(entity);
                }
            }

            entityid_generator_.entityid_delete(entity);
        }

        Entity CreateEntityFrom(EntityId entity)
        {
            return Entity(this, entity);
        }

        LiteEntity CreateLiteEntityFrom(EntityId entity)
        {
            return LiteEntity(this, entity);
        }

    public:
        template <typename ComponentType>
        typename detail::ComponentStorageInfo<ComponentType>::StorageType* GetComponentStorageOrCreateIt();

        template <typename ComponentType>
        void CreateComponentStorage()
        {
            GetComponentStorageOrCreateIt<ComponentType>();
        }

        template <typename ComponentType>
        const typename detail::ComponentStorageInfo<ComponentType>::StorageType* get_component_storage() const;

        template <typename ComponentType>
        typename detail::ComponentStorageInfo<ComponentType>::StorageType* get_component_storage()
        {
            return const_cast<typename detail::ComponentStorageInfo<ComponentType>::StorageType*>(const_cast<const Registry*>(this)->get_component_storage<ComponentType>());
        }

    public:
        template <typename ComponentType, typename... Args>
        ComponentType& Add(EntityId entity, Args&&... args);

        template <typename ComponentType>
        bool has(EntityId entity);

        template <typename ComponentType, typename... ComponentTypes>
        bool has_all(EntityId entity);

    public:
        template <typename ComponentType>
        const ComponentType& get(EntityId entity) const;
        template <typename ComponentType>
        ComponentType& get(EntityId entity);

        template <typename ComponentType>
        const ComponentType* GetPtr(EntityId entity) const;
        template <typename ComponentType>
        ComponentType* GetPtr(EntityId entity);

        template <typename... ComponentTypes>
        std::tuple<const ComponentTypes&...> get_all(EntityId entity) const;
        template <typename... ComponentTypes>
        std::tuple<ComponentTypes&...> get_all(EntityId entity);

    public:
        template<typename... ComponentTypes>
        BasicView<ComponentTypes...> View()
        {
            return BasicView<ComponentTypes...>(*this);
        }

    public:
        // Function = std::function<void(EntityId)>
        template<typename Function>
        void foreach_entities(Function&& function);

        // Function = std::function<void(EntityId, ComponentType& component)>
        template<typename ComponentType, typename Function>
        void foreach_unique_component(Function&& function);
        // Function = std::function<void(EntityId, ComponentType& component)>
        template<typename ComponentType, typename Function>
        void foreach_unique_component(Function&& function) const;
        // Function = std::function<void(EntityId, ComponentTypes&... components)>
        template<typename... ComponentTypes, typename Function>
        void foreach_components(Function&& function);
        // Function = std::function<void(EntityId, ComponentTypes&... components)>
        template<typename... ComponentTypes, typename Function>
        void foreach_components(Function&& function) const;

    public:
        const auto& each_entities()
        {
            return entityid_generator_.get_alives_entities();
        }

        template<typename ComponentType>
        decltype(auto) each_entities_with();
        template<typename ComponentType>
        decltype(auto) each_entities_with() const;
        
        // TODO
        // template<typename ComponentType>
        // decltype(auto) each_unique_component();
        // template<typename ComponentType>
        // decltype(auto) each_unique_component() const;
    };
}

#include "registry-inl.h"
#include "registry_foreach-inl.h"
#include "registry_iterator-inl.h"

#include "views/basic_view-inl-r.h"

#include "lecs/entity/entity-inl-r.h"
#include "lecs/entity/lite_entity-inl-r.h"