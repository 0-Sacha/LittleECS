#pragma once

#include "LittleECS/Registry/IComponentStorage.h"

#include "LittleECS/Detail/TypeTraits.h"
#include "LittleECS/Detail/ApplicableFunction.h"
#include "LittleECS/Detail/Iterable.h"

#include <array>
#include <tuple>
#include <functional>

namespace LECS
{
    class Registry;

    template <typename... ViewComponentTypes>
    class BasicConstView
    {
    public:
        using M_Type = BasicConstView<ViewComponentTypes...>;

        template <std::size_t I>
        using TypeAt = typename Detail::GetTypeAt<I, ViewComponentTypes...>::Type;
        template <typename ComponentType>
        using TypeIndex = typename Detail::GetTypeIndex<ComponentType, 0, ViewComponentTypes...>;

        template <std::size_t I>
        const typename Detail::ComponentStorageInfo<TypeAt<I>>::StorageType* GetComponentStorageAt() const
        {
            return reinterpret_cast<const typename Detail::ComponentStorageInfo<TypeAt<I>>::StorageType*>(m_LinkToComponentContainer[I]);
        }

    public:
        BasicConstView(const Registry& linkedRegistry)
            : m_LinkedRegistry(linkedRegistry)
            , m_LinkToComponentContainer{ nullptr }
        {
            Refresh();
        }

    protected:
        const Registry& m_LinkedRegistry;
        std::array<const Detail::IComponentStorage*, sizeof...(ViewComponentTypes)> m_LinkToComponentContainer;

    private:
        template <std::size_t I, typename Component, typename... ComponentRest>
        void RefreshRegistryLink();

        void Refresh()
        {
            return RefreshRegistryLink<0, ViewComponentTypes...>();
        }

    public:
        template <typename ComponentType>
        bool Has(EntityId entity) const
        {
            auto storage = GetComponentStorageAt<TypeIndex<ComponentType>::Index>();
            if (storage == nullptr)
                return false;
            return storage->HasThisComponent(entity);
        }

        template <typename ComponentType, typename... ComponentTypes>
        bool HasAll(EntityId entity) const
        {
            if constexpr (sizeof...(ComponentTypes) == 0)
                return Has<ComponentType>(entity);
            else
                return Has<ComponentType>(entity) && HasAll<ComponentTypes...>(entity);
        }

    public:
        template <typename ComponentType>
        const ComponentType& Get(EntityId entity) const
        {
            auto storage = GetComponentStorageAt<TypeIndex<ComponentType>::Index>();
            LECS_ASSERT(storage, "This entity doesn't have this component")
            return storage->GetComponentOfEntity(entity);
        }
        template <typename ComponentType>
        const ComponentType* GetPtr(EntityId entity) const
        {
            auto storage = GetComponentStorageAt<TypeIndex<ComponentType>::Index>();
            LECS_ASSERT(storage, "This entity doesn't have this component")
            return storage->GetComponentOfEntityPtr(entity);
        }
        template <typename... ComponentTypes>
        std::tuple<const ComponentTypes&...> GetAll(EntityId entity) const
        {
            return std::tuple<const ComponentTypes&...>(Get<ComponentTypes>(entity)...);
        }

    public:
        // Function = std::function<void(EntityId)>
        template<typename Function>
        requires (Detail::IsApplicable<Function, EntityId>::Value)
        void ForEachEntities(Function&& function) const
        {
            return ForEachComponents<ViewComponentTypes...>(std::forward<Function>(function));
        }

        // Function = std::function<void(EntityId, ComponentTypeEach& component)>
        template <typename ComponentTypeEach, typename Function>
        void ForEachUniqueComponent(Function&& function) const;
        // Function = std::function<void(EntityId, ComponentTypeRanged& component, ComponentTypesEach&... components)>
        template <typename ComponentTypeRanged, typename... ComponentTypesEach, typename Function>
        void ForEachComponents(Function&& function) const;
    
    public:
        decltype(auto) EachEntities() const
        {
            return EachEntitiesWithAll<ViewComponentTypes...>();
        }
        template <typename ComponentTypeEach>
        decltype(auto) EachEntitiesWith() const;
        template <typename ComponentTypeEach, typename... ComponentTypesEach>
        decltype(auto) EachEntitiesWithAll() const;

        template <typename ComponentTypeEach>
        decltype(auto) EachUniqueComponent() const;
        template <typename RangeComponent, typename... ComponentTypesEach>
        decltype(auto) EachComponents() const;
    };

    template <typename... ViewComponentTypes>
    class BasicView : public BasicConstView<ViewComponentTypes...>
    {
    public:
        using Base = BasicConstView<ViewComponentTypes...>;
        using M_Type = BasicView<ViewComponentTypes...>;

        template <std::size_t I>
        using TypeAt = typename Detail::GetTypeAt<I, ViewComponentTypes...>::Type;
        template <typename ComponentType>
        using TypeIndex = typename Detail::GetTypeIndex<ComponentType, 0, ViewComponentTypes...>;
        
        template <std::size_t I>
        const typename Detail::ComponentStorageInfo<TypeAt<I>>::StorageType* GetComponentStorageAt() const
        {
            return Base::template GetComponentStorageAt<I>();
        }
        template <std::size_t I>
        typename Detail::ComponentStorageInfo<TypeAt<I>>::StorageType* GetComponentStorageAt()
        {
            return const_cast<typename Detail::ComponentStorageInfo<TypeAt<I>>::StorageType*>(Base::template GetComponentStorageAt<I>());
        }

        Registry& GetRegistry()
        {
            return const_cast<Registry&>(Base::m_LinkedRegistry);
        }

    public:
        BasicView(Registry& linkedRegistry)
            : Base(linkedRegistry)
        {}

    public:
        template <typename ComponentType>
        inline bool Has(EntityId entity) const
        {
            return Base::template Has<ComponentType>(entity);
        }
        template <typename ComponentType, typename... ComponentTypes>
        inline bool HasAll(EntityId entity) const
        {
            return Base::template HasAll<ComponentType, ComponentTypes...>(entity);
        }

        template <typename ComponentType>
        const ComponentType& Get(EntityId entity) const
        {
            return Base::template Get<ComponentType>(entity);
        }
        template <typename ComponentType>
        ComponentType& Get(EntityId entity)
        {
            auto storage = GetComponentStorageAt<TypeIndex<ComponentType>::Index>();
            LECS_ASSERT(storage, "This entity doesn't have this component")
            return storage->GetComponentOfEntity(entity);
        }

        template <typename ComponentType>
        const ComponentType& GetPtr(EntityId entity) const
        {
            return Base::template GetPtr<ComponentType>(entity);
        }
        template <typename ComponentType>
        ComponentType* GetPtr(EntityId entity)
        {
            auto storage = GetComponentStorageAt<TypeIndex<ComponentType>::Index>();
            LECS_ASSERT(storage, "This entity doesn't have this component")
            return &storage->GetComponentOfEntity(entity);
        }

        template <typename... ComponentTypes>
        std::tuple<const ComponentTypes&...> GetAll(EntityId entity) const
        {
            return Base::template GetAll<ComponentTypes...>(entity);
        }
        template <typename... ComponentTypes>
        std::tuple<ComponentTypes&...> GetAll(EntityId entity)
        {
            return std::tuple<ComponentTypes&...>(Get<ComponentTypes>(entity)...);
        }

    public:
        // Function = std::function<void(EntityId)>
        template<typename Function>
        requires (Detail::IsApplicable<Function, EntityId>::Value)
        void ForEachEntities(Function&& function) const
        {
            return Base::template ForEachEntities<Function>(std::forward<Function>(function));
        }
        // Function = std::function<void(EntityId, ComponentTypeEach& component)>
        template <typename ComponentTypeEach, typename Function>
        void ForEachUniqueComponent(Function&& function) const
        {
            return Base::template ForEachComponents<ComponentTypeEach, Function>(std::forward<Function>(function));
        }
        // Function = std::function<void(EntityId, ComponentTypeRanged& component, ComponentTypesEach&... components)>
        template <typename ComponentTypeRanged, typename... ComponentTypesEach, typename Function>
        void ForEachComponents(Function&& function) const
        {
            return Base::template ForEachComponents<ComponentTypeRanged, ComponentTypesEach..., Function>(std::forward<Function>(function));
        }

        // Function = std::function<void(EntityId, ComponentTypeEach& component)>
        template <typename ComponentTypeEach, typename Function>
        void ForEachUniqueComponent(Function&& function);
        // Function = std::function<void(EntityId, ComponentTypeRanged& component, ComponentTypesEach&... components)>
        template <typename ComponentTypeRanged, typename... ComponentTypesEach, typename Function>
        void ForEachComponents(Function&& function);

    public:
        using Base::EachEntities;
        template <typename ComponentTypeEach>
        decltype(auto) EachEntitiesWith() const
        {
            return Base::template EachEntitiesWith<ComponentTypeEach>();
        }
        template <typename RangeComponent, typename... ComponentTypesEach>
        decltype(auto) EachEntitiesWithAll() const
        {
            return Base::template EachEntitiesWithAll<RangeComponent, ComponentTypesEach...>();
        }

        template <typename ComponentTypeEach>
        decltype(auto) EachUniqueComponent() const
        {
            return Base::template EachUniqueComponent<ComponentTypeEach>();
        }
        template <typename RangeComponent, typename... ComponentTypesEach>
        decltype(auto) EachComponents() const
        {
            return Base::template EachComponents<RangeComponent, ComponentTypesEach...>();
        }

        template <typename ComponentTypeEach>
        decltype(auto) EachUniqueComponent();
        template <typename RangeComponent, typename... ComponentTypesEach>
        decltype(auto) EachComponents();
    };
}

#include "BasicViewEach-inl.h"
#include "BasicViewForEach-inl.h"
