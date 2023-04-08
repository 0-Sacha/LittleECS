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
            CreateRegistryLink();
        }

    private:
        Registry& m_LinkedRegistry;
        std::array<Detail::IComponentStorage*, sizeof...(ViewComponentTypes)> m_LinkToComponentContainer;

    private:
        template <std::size_t I, typename Component, typename... ComponentRest>
        void CreateRegistryLinkImpl();

        void CreateRegistryLink()
        {
            return CreateRegistryLinkImpl<0, ViewComponentTypes...>();
        }

    public:
        template <typename ComponentType, typename... ComponentTypes>
        bool EntityHasAll(EntityId entity) const
        {
			auto storage = GetComponentStorageAt<TypeIndex<ComponentType>::Index>();
            if (storage == nullptr)
                return false;
			if constexpr (sizeof...(ComponentTypes) > 0)
                return storage->EntityHasThisComponent(entity) && EntityHasAll<ComponentTypes...>(entity);
            else
                return storage->EntityHasThisComponent(entity);
        }

    private:
		template <typename ComponentType>
        ComponentType& GetComponentType(EntityId entity)
        {
			auto storage = GetComponentStorageAt<TypeIndex<ComponentType>::Index>();
			LECS_ASSERT(storage, "This entity doesn't have this component")
			return storage->GetComponentOfEntity(entity);
		}
		template <typename ComponentType>
		const ComponentType& GetComponentType(EntityId entity) const
		{
			auto storage = GetComponentStorageAt<TypeIndex<ComponentType>::Index>();
            LECS_ASSERT(storage, "This entity doesn't have this component")
            return storage->GetComponentOfEntity(entity);
		}

	public:
		template <typename... ComponentTypes>
		std::tuple<ComponentTypes&...> GetComponentTuple(EntityId entity)
		{
            return std::tuple<ComponentTypes&...>(GetComponentType<ComponentTypes>(entity)...);
		}

        template <typename... ComponentTypes>
		std::tuple<const ComponentTypes&...> GetComponentTuple(EntityId entity) const
		{
            return std::tuple<const ComponentTypes&...>(GetComponentType<ComponentTypes>(entity)...);
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
    };
}
