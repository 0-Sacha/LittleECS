#pragma once

#include "LittleECS/Registry/IComponentStorage.h"

#include "LittleECS/Detail/TypeParameterPack.h"

#include <array>
#include <tuple>
#include <functional>

namespace LittleECS
{
    class Registry;
    
    template <typename... ViewComponentTypes>
    class BasicView
    {
    public:
        using M_Type = BasicView<ViewComponentTypes...>;

        template <std::size_t I>
        using TypeAt = typename Detail::GetTypeAt<I, ViewComponentTypes...>::Type;

        template <typename ComponentType>
        using TypeIndex = typename Detail::GetTypeIndex<ComponentType, 0, ViewComponentTypes...>;

        template <std::size_t I>
        typename Detail::ComponentStorageInfo<TypeAt<I>>::StorageType* GetComponentStorageAt()
        {
            return reinterpret_cast<typename Detail::ComponentStorageInfo<TypeAt<I>>::StorageType*>(m_LinkToComponentContainer[I]);
        }

        template <std::size_t I>
        const typename Detail::ComponentStorageInfo<TypeAt<I>>::StorageType* GetComponentStorageAt() const
        {
            return reinterpret_cast<const typename Detail::ComponentStorageInfo<TypeAt<I>>::StorageType*>(m_LinkToComponentContainer[I]);
        }

    public:
        BasicView(Registry& linkedRegistry)
            : m_LinkedRegistry(linkedRegistry)
            , m_LinkToComponentContainer{ nullptr }
        {
            Refresh();
        }

    private:
        Registry& m_LinkedRegistry;
        std::array<Detail::IComponentStorage*, sizeof...(ViewComponentTypes)> m_LinkToComponentContainer;

    private:
        template <std::size_t I, typename Component, typename... ComponentRest>
        void CreateRegistryLink();

        void Refresh()
        {
            return CreateRegistryLink<0, ViewComponentTypes...>();
        }

    public:
        template <typename ComponentType>
        bool EntityHas(EntityId entity) const
        {
			auto storage = GetComponentStorageAt<TypeIndex<ComponentType>::Index>();
            if (storage == nullptr)
                return false;
            return storage->EntityHasThisComponent(entity);
        }

        template <typename ComponentType, typename... ComponentTypes>
        bool EntityHasAll(EntityId entity) const
        {
			if constexpr (sizeof...(ComponentTypes) == 0)
                return EntityHas<ComponentType>(entity);
            else
                return EntityHas<ComponentType>(entity) && EntityHasAll<ComponentTypes...>(entity);
        }

    public:
		template <typename ComponentType>
        ComponentType& GetComponent(EntityId entity)
        {
			auto storage = GetComponentStorageAt<TypeIndex<ComponentType>::Index>();
			LECS_ASSERT(storage, "This entity doesn't have this component")
			return storage->GetComponentOfEntity(entity);
		}
		template <typename ComponentType>
		const ComponentType& GetComponent(EntityId entity) const
		{
			auto storage = GetComponentStorageAt<TypeIndex<ComponentType>::Index>();
            LECS_ASSERT(storage, "This entity doesn't have this component")
            return storage->GetComponentOfEntity(entity);
		}
		template <typename... ComponentTypes>
		std::tuple<ComponentTypes&...> GetComponentTuple(EntityId entity)
		{
            return std::tuple<ComponentTypes&...>(GetComponent<ComponentTypes>(entity)...);
		}
        template <typename... ComponentTypes>
		std::tuple<const ComponentTypes&...> GetComponentTuple(EntityId entity) const
		{
            return std::tuple<const ComponentTypes&...>(GetComponent<ComponentTypes>(entity)...);
		}
        template <typename... ComponentTypes>
		decltype(auto) GetComponents(EntityId entity)
		{
            if constexpr (sizeof...(ComponentTypes) == 0)
                return GetComponentTuple<ViewComponentTypes...>(entity);
            else if constexpr (sizeof...(ComponentTypes) == 1)
                return GetComponent<ComponentTypes...>(entity);
            else if constexpr (sizeof...(ComponentTypes) > 1)
                return GetComponentTuple<ComponentTypes...>(entity);
		}
        template <typename... ComponentTypes>
		decltype(auto) GetComponents(EntityId entity) const
		{
            if constexpr (sizeof...(ComponentTypes) == 0)
                return GetComponentTuple<ViewComponentTypes...>(entity);
            else if constexpr (sizeof...(ComponentTypes) == 1)
                return GetComponent<ComponentTypes...>(entity);
            else if constexpr (sizeof...(ComponentTypes) > 1)
                return GetComponentTuple<ComponentTypes...>(entity);
		}

    public:
        // Function = std::function<void(EntityId, ComponentTypeRanged& component, ComponentTypesEach&... components)>
        template <typename ComponentTypeRanged, typename... ComponentTypesEach, typename Function>
        void ForEachComponents(Function&& function);
        // Function = std::function<void(EntityId, ComponentTypeEach& component)>
        template <typename ComponentTypeEach, typename Function>
        void ForEachUniqueComponent(Function&& function);
        // Function = std::function<void(EntityId, ComponentTypeRanged& component, ComponentTypesEach&... components)>
        template <typename ComponentTypeRanged, typename... ComponentTypesEach, typename Function>
        void ForEachComponents(Function&& function) const;
        // Function = std::function<void(EntityId, ComponentTypeEach& component)>
        template <typename ComponentTypeEach, typename Function>
        void ForEachUniqueComponent(Function&& function) const;
    
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
        decltype(auto) EachUniqueComponent();
        template <typename ComponentTypeEach>
        decltype(auto) EachUniqueComponent() const;
	    template <typename RangeComponent, typename... ComponentTypesEach>
        decltype(auto) EachComponents();
	    template <typename RangeComponent, typename... ComponentTypesEach>
        decltype(auto) EachComponents() const;
    };

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

    private:
        const Registry& m_LinkedRegistry;
        std::array<const Detail::IComponentStorage*, sizeof...(ViewComponentTypes)> m_LinkToComponentContainer;

    private:
        template <std::size_t I, typename Component, typename... ComponentRest>
        void CreateRegistryLink();

        void Refresh()
        {
            return CreateRegistryLink<0, ViewComponentTypes...>();
        }

    public:
        template <typename ComponentType>
        bool EntityHas(EntityId entity) const
        {
			auto storage = GetComponentStorageAt<TypeIndex<ComponentType>::Index>();
            if (storage == nullptr)
                return false;
            return storage->EntityHasThisComponent(entity);
        }

        template <typename ComponentType, typename... ComponentTypes>
        bool EntityHasAll(EntityId entity) const
        {
			if constexpr (sizeof...(ComponentTypes) == 0)
                return EntityHas<ComponentType>(entity);
            else
                return EntityHas<ComponentType>(entity) && EntityHasAll<ComponentTypes...>(entity);
        }

    public:
		template <typename ComponentType>
		const ComponentType& GetComponent(EntityId entity) const
		{
			auto storage = GetComponentStorageAt<TypeIndex<ComponentType>::Index>();
            LECS_ASSERT(storage, "This entity doesn't have this component")
            return storage->GetComponentOfEntity(entity);
		}
        template <typename... ComponentTypes>
		std::tuple<const ComponentTypes&...> GetComponentTuple(EntityId entity) const
		{
            return std::tuple<const ComponentTypes&...>(GetComponent<ComponentTypes>(entity)...);
		}
        template <typename... ComponentTypes>
		decltype(auto) GetComponents(EntityId entity) const
		{
            if constexpr (sizeof...(ComponentTypes) == 0)
                return GetComponentTuple<ViewComponentTypes...>(entity);
            else if constexpr (sizeof...(ComponentTypes) == 1)
                return GetComponent<ComponentTypes...>(entity);
            else if constexpr (sizeof...(ComponentTypes) > 1)
                return GetComponentTuple<ComponentTypes...>(entity);
		}

    public:
        // Function = std::function<void(EntityId, ComponentTypeRanged& component, ComponentTypesEach&... components)>
        template <typename ComponentTypeRanged, typename... ComponentTypesEach, typename Function>
        void ForEachComponents(Function&& function) const;
        // Function = std::function<void(EntityId, ComponentTypeEach& component)>
        template <typename ComponentTypeEach, typename Function>
        void ForEachUniqueComponent(Function&& function) const;
    
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
}
