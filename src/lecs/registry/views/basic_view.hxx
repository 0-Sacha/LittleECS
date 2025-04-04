#pragma once

#include "lecs/registry/component_storage.hxx"

#include "lecs/detail/type_traits.hxx"
#include "lecs/detail/is_invocable.hxx"
#include "lecs/detail/iterable.hxx"

#include <array>
#include <tuple>
#include <functional>

namespace lecs {
    class Registry;

    template <typename... ViewComponentTypes>
    class BasicConstView {
    public:
        using M_Type = BasicConstView<ViewComponentTypes...>;

        template <std::size_t I>
        using TypeAt = typename detail::get_type_at<I, ViewComponentTypes...>::type;
        template <typename ComponentType>
        using TypeIndex = typename detail::get_type_index<ComponentType, 0, ViewComponentTypes...>;

        template <std::size_t I>
        const typename detail::ComponentStorageInfo<TypeAt<I>>::StorageType* get_component_storage_at() const {
            return reinterpret_cast<const typename detail::ComponentStorageInfo<TypeAt<I>>::StorageType*>(link_to_component_container_[I]);
        }

    public:
        BasicConstView(const Registry& linked_registry) : linked_registry_(linked_registry), link_to_component_container_{nullptr} {
            refresh();
        }

    protected:
        const Registry&                                                             linked_registry_;
        std::array<const detail::IComponentStorage*, sizeof...(ViewComponentTypes)> link_to_component_container_;

    private:
        template <std::size_t I, typename Component, typename... ComponentRest>
        void refresh_registry_link();

        void refresh() {
            return refresh_registry_link<0, ViewComponentTypes...>();
        }

    public:
        template <typename ComponentType>
        bool has(EntityId entity) const {
            auto storage = get_component_storage_at<TypeIndex<ComponentType>::index>();
            if (storage == nullptr) return false;
            return storage->has_this_component(entity);
        }

        template <typename ComponentType, typename... ComponentTypes>
        bool has_all(EntityId entity) const {
            if constexpr (sizeof...(ComponentTypes) == 0)
                return has<ComponentType>(entity);
            else
                return has<ComponentType>(entity) && has_all<ComponentTypes...>(entity);
        }

    public:
        template <typename ComponentType>
        const ComponentType& get(EntityId entity) const {
            auto storage = get_component_storage_at<TypeIndex<ComponentType>::index>();
            LECS_ASSERT(storage, "This entity doesn't have this component")
            return storage->get_entity_componenttype(entity);
        }
        template <typename ComponentType>
        const ComponentType* get_ptr(EntityId entity) const {
            auto storage = get_component_storage_at<TypeIndex<ComponentType>::index>();
            LECS_ASSERT(storage, "This entity doesn't have this component")
            return storage->get_entity_componenttype_ptr(entity);
        }
        template <typename... ComponentTypes>
        std::tuple<const ComponentTypes&...> get_all(EntityId entity) const {
            return std::tuple<const ComponentTypes&...>(get<ComponentTypes>(entity)...);
        }

    public:
        // Function = std::function<void(EntityId)>
        template <typename Function>
            requires(detail::is_invocable<Function, EntityId>::value)
        void foreach_entities(Function&& function) const {
            return foreach_components<ViewComponentTypes...>(std::forward<Function>(function));
        }

        // Function = std::function<void(EntityId, ComponentTypeEach& component)>
        template <typename ComponentTypeEach, typename Function>
        void foreach_unique_component(Function&& function) const;
        // Function = std::function<void(EntityId, ComponentTypeRanged& component, ComponentTypesEach&... components)>
        template <typename ComponentTypeRanged, typename... ComponentTypesEach, typename Function>
        void foreach_components(Function&& function) const;

    public:
        decltype(auto) each_entities() const {
            return each_entities_with_all<ViewComponentTypes...>();
        }
        template <typename ComponentTypeEach>
        decltype(auto) each_entities_with() const;
        template <typename ComponentTypeEach, typename... ComponentTypesEach>
        decltype(auto) each_entities_with_all() const;

        template <typename ComponentTypeEach>
        decltype(auto) each_unique_component() const;
        template <typename RangeComponent, typename... ComponentTypesEach>
        decltype(auto) each_components() const;
    };

    template <typename... ViewComponentTypes>
    class BasicView : public BasicConstView<ViewComponentTypes...> {
    public:
        using Base   = BasicConstView<ViewComponentTypes...>;
        using M_Type = BasicView<ViewComponentTypes...>;

        template <std::size_t I>
        using TypeAt = typename detail::get_type_at<I, ViewComponentTypes...>::type;
        template <typename ComponentType>
        using TypeIndex = typename detail::get_type_index<ComponentType, 0, ViewComponentTypes...>;

        template <std::size_t I>
        const typename detail::ComponentStorageInfo<TypeAt<I>>::StorageType* get_component_storage_at() const {
            return Base::template get_component_storage_at<I>();
        }
        template <std::size_t I>
        typename detail::ComponentStorageInfo<TypeAt<I>>::StorageType* get_component_storage_at() {
            return const_cast<typename detail::ComponentStorageInfo<TypeAt<I>>::StorageType*>(Base::template get_component_storage_at<I>());
        }

        Registry& get_registry() {
            return const_cast<Registry&>(Base::linked_registry_);
        }

    public:
        BasicView(Registry& linked_registry) : Base(linked_registry) {}

    public:
        template <typename ComponentType>
        inline bool has(EntityId entity) const {
            return Base::template has<ComponentType>(entity);
        }
        template <typename ComponentType, typename... ComponentTypes>
        inline bool has_all(EntityId entity) const {
            return Base::template has_all<ComponentType, ComponentTypes...>(entity);
        }

        template <typename ComponentType>
        const ComponentType& get(EntityId entity) const {
            return Base::template get<ComponentType>(entity);
        }
        template <typename ComponentType>
        ComponentType& get(EntityId entity) {
            auto storage = get_component_storage_at<TypeIndex<ComponentType>::index>();
            LECS_ASSERT(storage, "This entity doesn't have this component")
            return storage->get_entity_componenttype(entity);
        }

        template <typename ComponentType>
        const ComponentType& get_ptr(EntityId entity) const {
            return Base::template get_ptr<ComponentType>(entity);
        }
        template <typename ComponentType>
        ComponentType* get_ptr(EntityId entity) {
            auto storage = get_component_storage_at<TypeIndex<ComponentType>::index>();
            LECS_ASSERT(storage, "This entity doesn't have this component")
            return &storage->get_entity_componenttype(entity);
        }

        template <typename... ComponentTypes>
        std::tuple<const ComponentTypes&...> get_all(EntityId entity) const {
            return Base::template get_all<ComponentTypes...>(entity);
        }
        template <typename... ComponentTypes>
        std::tuple<ComponentTypes&...> get_all(EntityId entity) {
            return std::tuple<ComponentTypes&...>(get<ComponentTypes>(entity)...);
        }

    public:
        // Function = std::function<void(EntityId)>
        template <typename Function>
            requires(detail::is_invocable<Function, EntityId>::value)
        void foreach_entities(Function&& function) const {
            return Base::template foreach_entities<Function>(std::forward<Function>(function));
        }
        // Function = std::function<void(EntityId, ComponentTypeEach& component)>
        template <typename ComponentTypeEach, typename Function>
        void foreach_unique_component(Function&& function) const {
            return Base::template foreach_components<ComponentTypeEach, Function>(std::forward<Function>(function));
        }
        // Function = std::function<void(EntityId, ComponentTypeRanged& component, ComponentTypesEach&... components)>
        template <typename ComponentTypeRanged, typename... ComponentTypesEach, typename Function>
        void foreach_components(Function&& function) const {
            return Base::template foreach_components<ComponentTypeRanged, ComponentTypesEach..., Function>(std::forward<Function>(function));
        }

        // Function = std::function<void(EntityId, ComponentTypeEach& component)>
        template <typename ComponentTypeEach, typename Function>
        void foreach_unique_component(Function&& function);
        // Function = std::function<void(EntityId, ComponentTypeRanged& component, ComponentTypesEach&... components)>
        template <typename ComponentTypeRanged, typename... ComponentTypesEach, typename Function>
        void foreach_components(Function&& function);

    public:
        using Base::each_entities;
        template <typename ComponentTypeEach>
        decltype(auto) each_entities_with() const {
            return Base::template each_entities_with<ComponentTypeEach>();
        }
        template <typename RangeComponent, typename... ComponentTypesEach>
        decltype(auto) each_entities_with_all() const {
            return Base::template each_entities_with_all<RangeComponent, ComponentTypesEach...>();
        }

        template <typename ComponentTypeEach>
        decltype(auto) each_unique_component() const {
            return Base::template each_unique_component<ComponentTypeEach>();
        }
        template <typename RangeComponent, typename... ComponentTypesEach>
        decltype(auto) each_components() const {
            return Base::template each_components<RangeComponent, ComponentTypesEach...>();
        }

        template <typename ComponentTypeEach>
        decltype(auto) each_unique_component();
        template <typename RangeComponent, typename... ComponentTypesEach>
        decltype(auto) each_components();
    };
}  // namespace lecs
