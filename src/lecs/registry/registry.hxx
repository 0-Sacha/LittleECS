#pragma once

#include "lecs/detail/entityid_generator.hxx"

#include "component_storage.hxx"

#include "lecs/entity/entity.hxx"
#include "lecs/entity/lite_entity.hxx"

#include "views/basic_view.hxx"

#include <unordered_map>
#include <memory>

namespace lecs {
    class Registry {
    public:
        struct ComponentData {
            std::unique_ptr<detail::IComponentStorage> component_storage;
            std::function<void(EntityId)>              on_construct;
            std::function<void(EntityId)>              on_destruct;
        };
        using ComponentIdToComponentData = std::unordered_map<ComponentId::Type, ComponentData>;
        using ComponentIdGenerator       = detail::CompilerComponentIdGenerator;

    public:
        Registry() {}

    protected:
        ComponentIdToComponentData componentid_to_component_data_;
        detail::EntityIdGenerator  entityid_generator_;

        // Entity Management
    public:
        const detail::EntityIdGenerator& get_entityid_generator() const {
            return entityid_generator_;
        }
        const ComponentIdToComponentData& get_componentid_to_componentdata() const {
            return componentid_to_component_data_;
        }

        bool registry_has(EntityId entity) const {
            return entityid_generator_.has_entityid(entity);
        }

        EntityId create_entityid() {
            return entityid_generator_.get_new_entityid();
        }

        void destroy_entityid(EntityId entity) {
            for (auto& container : componentid_to_component_data_) {
                if (container.second.component_storage->has_this_component_v(entity)) {
                    container.second.component_storage->remove_component_of_entity_v(entity);
                }
            }

            entityid_generator_.entityid_delete(entity);
        }

        Entity create_entity_from(EntityId entity) {
            return Entity(this, entity);
        }

        LiteEntity create_lite_entity_from(EntityId entity) {
            return LiteEntity(this, entity);
        }

    public:
        template <typename ComponentType>
        typename detail::ComponentStorageInfo<ComponentType>::StorageType* get_component_storage_or_create_it();

        template <typename ComponentType>
        void create_component_storage() {
            get_component_storage_or_create_it<ComponentType>();
        }

        template <typename ComponentType>
        const typename detail::ComponentStorageInfo<ComponentType>::StorageType* get_component_storage() const;

        template <typename ComponentType>
        typename detail::ComponentStorageInfo<ComponentType>::StorageType* get_component_storage() {
            return const_cast<typename detail::ComponentStorageInfo<ComponentType>::StorageType*>(const_cast<const Registry*>(this)->get_component_storage<ComponentType>());
        }

    public:
        template <typename ComponentType, typename... Args>
        ComponentType& add(EntityId entity, Args&&... args);

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
        const ComponentType* get_ptr(EntityId entity) const;
        template <typename ComponentType>
        ComponentType* get_ptr(EntityId entity);

        template <typename... ComponentTypes>
        std::tuple<const ComponentTypes&...> get_all(EntityId entity) const;
        template <typename... ComponentTypes>
        std::tuple<ComponentTypes&...> get_all(EntityId entity);

    public:
        template <typename... ComponentTypes>
        BasicView<ComponentTypes...> view() {
            return BasicView<ComponentTypes...>(*this);
        }

    public:
        // Function = std::function<void(EntityId)>
        template <typename Function>
        void foreach_entities(Function&& function);

        // Function = std::function<void(EntityId, ComponentType& component)>
        template <typename ComponentType, typename Function>
        void foreach_unique_component(Function&& function);
        // Function = std::function<void(EntityId, ComponentType& component)>
        template <typename ComponentType, typename Function>
        void foreach_unique_component(Function&& function) const;
        // Function = std::function<void(EntityId, ComponentTypes&... components)>
        template <typename... ComponentTypes, typename Function>
        void foreach_components(Function&& function);
        // Function = std::function<void(EntityId, ComponentTypes&... components)>
        template <typename... ComponentTypes, typename Function>
        void foreach_components(Function&& function) const;

    public:
        const auto& each_entities() {
            return entityid_generator_.get_alives_entities();
        }

        template <typename ComponentType>
        decltype(auto) each_entities_with();
        template <typename ComponentType>
        decltype(auto) each_entities_with() const;

        // TODO
        // template<typename ComponentType>
        // decltype(auto) each_unique_component();
        // template<typename ComponentType>
        // decltype(auto) each_unique_component() const;
    };
}  // namespace lecs

#include "registry_impl.hxx"
#include "registry_foreach_impl.hxx"
#include "registry_iterator_impl.hxx"

#include "views/basic_view_impl.hxx"
#include "views/basic_view_each_impl.hxx"
#include "views/basic_view_foreach_impl.hxx"

#include "lecs/entity/entity_impl.hxx"
#include "lecs/entity/lite_entity_impl.hxx"