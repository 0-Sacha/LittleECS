#pragma once

#include "registry.hxx"

namespace lecs {
    template <typename ComponentType>
    typename detail::ComponentStorageInfo<ComponentType>::StorageType* Registry::get_component_storage_or_create_it() {
        ComponentId componentId = ComponentIdGenerator::get_typeid<ComponentType>();

        if (componentid_to_component_data_.contains(componentId) == true) {
            detail::IComponentStorage* componentStorageBasic = componentid_to_component_data_[componentId].component_storage.get();
            return reinterpret_cast<typename detail::ComponentStorageInfo<ComponentType>::StorageType*>(componentStorageBasic);
        } else {
            std::unique_ptr<detail::IComponentStorage>& componentStorageRef = componentid_to_component_data_[componentId].component_storage =
                std::make_unique<typename detail::ComponentStorageInfo<ComponentType>::StorageType>();
            detail::IComponentStorage* componentStorageBasic = componentStorageRef.get();
            return reinterpret_cast<typename detail::ComponentStorageInfo<ComponentType>::StorageType*>(componentStorageBasic);
        }

        return nullptr;
    }

    template <typename ComponentType>
    const typename detail::ComponentStorageInfo<ComponentType>::StorageType* Registry::get_component_storage() const {
        ComponentId componentId = ComponentIdGenerator::get_typeid<ComponentType>();

        const auto component_storage = componentid_to_component_data_.find(componentId);

        LECS_ASSERT(component_storage != componentid_to_component_data_.end(), "This ComponentStorage is not part of this registry")

        const detail::IComponentStorage* componentStorageBasic = component_storage->second.component_storage.get();
        return reinterpret_cast<const typename detail::ComponentStorageInfo<ComponentType>::StorageType*>(componentStorageBasic);
    }

    template <typename ComponentType, typename... Args>
    ComponentType& Registry::add(EntityId entity, Args&&... args) {
        typename detail::ComponentStorageInfo<ComponentType>::StorageType* component_storage = get_component_storage_or_create_it<ComponentType>();
        LECS_ASSERT(component_storage != nullptr, "This ComponentStorage is not part of this registry")
        return component_storage->add_component_to_entity(entity, std::forward<Args>(args)...);
    }

    template <typename ComponentType>
    bool Registry::has(EntityId entity) {
        ComponentId componentId = ComponentIdGenerator::get_typeid<ComponentType>();

        auto componentStoragefound = componentid_to_component_data_.find(componentId);

        if (componentStoragefound == componentid_to_component_data_.end()) return false;

        detail::IComponentStorage*                                         componentStorageBasic = componentStoragefound->second.component_storage.get();
        typename detail::ComponentStorageInfo<ComponentType>::StorageType* component_storage =
            reinterpret_cast<typename detail::ComponentStorageInfo<ComponentType>::StorageType*>(componentStorageBasic);

        if (component_storage == nullptr) return false;

        return component_storage->has_this_component(entity);
    }

    template <typename ComponentType, typename... ComponentTypes>
    bool Registry::has_all(EntityId entity) {
        if constexpr (sizeof...(ComponentTypes) == 0)
            return HasComponent<ComponentType>(entity);
        else
            return HasComponent<ComponentType>(entity) && HasAllComponents<ComponentTypes...>(entity);
    }

    template <typename ComponentType>
    const ComponentType& Registry::get(EntityId entity) const {
        const typename detail::ComponentStorageInfo<ComponentType>::StorageType* component_storage = get_component_storage<ComponentType>();
        LECS_ASSERT(component_storage != nullptr, "This ComponentStorage is not part of this registry")
        return component_storage->get_entity_componenttype(entity);
    }
    template <typename ComponentType>
    ComponentType& Registry::get(EntityId entity) {
        typename detail::ComponentStorageInfo<ComponentType>::StorageType* component_storage = get_component_storage<ComponentType>();
        LECS_ASSERT(component_storage != nullptr, "This ComponentStorage is not part of this registry")
        return component_storage->get_entity_componenttype(entity);
    }

    template <typename ComponentType>
    const ComponentType* Registry::get_ptr(EntityId entity) const {
        const typename detail::ComponentStorageInfo<ComponentType>::StorageType* component_storage = get_component_storage<ComponentType>();
        if (component_storage == nullptr) return nullptr;
        return component_storage->get_entity_componenttype_ptr(entity);
    }
    template <typename ComponentType>
    ComponentType* Registry::get_ptr(EntityId entity) {
        typename detail::ComponentStorageInfo<ComponentType>::StorageType* component_storage = get_component_storage<ComponentType>();
        if (component_storage == nullptr) return nullptr;
        return component_storage->get_entity_componenttype_ptr(entity);
    }

    template <typename... ComponentTypes>
    std::tuple<const ComponentTypes&...> Registry::get_all(EntityId entity) const {
        return std::tuple<const ComponentTypes&...>(get<ComponentTypes>(entity)...);
    }
    template <typename... ComponentTypes>
    std::tuple<ComponentTypes&...> Registry::get_all(EntityId entity) {
        return std::tuple<ComponentTypes&...>(get<ComponentTypes>(entity)...);
    }

    // Function = std::function<void(EntityId)>
    template <typename Function>
    void Registry::foreach_entities(Function&& function) {
        for (EntityId entity : entityid_generator_.get_alives_entities())
            function(entity);
    }

}  // namespace lecs