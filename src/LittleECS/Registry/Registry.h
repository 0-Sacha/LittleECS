#pragma once

#include "LittleECS/Detail/EntityIdGenerator.h"

#include "CompressedComponentStorage/CompressedComponentStorage.h"
#include "FastComponentStorage/FastComponentStorage.h"
#include "Views/BasicView.h"

#include <unordered_map>
#include <memory>

namespace LECS
{
	class Registry
	{
	public:
		using ComponentStorageRef = std::unique_ptr<Detail::IComponentStorage>;
		using ComponentsStoragesContainer = std::unordered_map<ComponentId::Type, ComponentStorageRef>;
		using ComponentIdGenerator = Detail::CompilerComponentIdGenerator;

	public:
		Registry() {}

	protected:
		ComponentsStoragesContainer m_ComponentsStoragesContainer;
		Detail::EntityIdGenerator m_EntityIdGenerator;

	// Entity Management
	public:
		const Detail::EntityIdGenerator& GetEntityIdGenerator() const
		{
			return m_EntityIdGenerator;
		}
		const ComponentsStoragesContainer& GetComponentsStoragesContainer() const
		{
			return m_ComponentsStoragesContainer;
		}

		EntityId CreateEntityId()
		{
			return m_EntityIdGenerator.GetNewEntityId();
		}

		void DestroyEntityId(EntityId entity)
		{
			for (auto& container : m_ComponentsStoragesContainer)
			{
				if (container.second->HasThisComponentV(entity))
				{
					container.second->RemoveComponentOfEntityV(entity);
				}
			}

			m_EntityIdGenerator.EntityIdDelete(entity);
		}

	public:
		template <typename ComponentType>
		typename Detail::ComponentStorageInfo<ComponentType>::StorageType* GetComponentStorageOrCreateIt();

		template <typename ComponentType>
		void CreateComponentStorage()
		{
			GetComponentStorageOrCreateIt<ComponentType>();
		}

		template <typename ComponentType>
		const typename Detail::ComponentStorageInfo<ComponentType>::StorageType* GetComponentStorage() const;

		template <typename ComponentType>
		typename Detail::ComponentStorageInfo<ComponentType>::StorageType* GetComponentStorage()
		{
			return const_cast<typename Detail::ComponentStorageInfo<ComponentType>::StorageType*>(const_cast<const Registry*>(this)->GetComponentStorage<ComponentType>());
		}

	public:
		template <typename ComponentType, typename... Args>
		ComponentType& Add(EntityId entity, Args&&... args);

		template <typename ComponentType>
		bool Has(EntityId entity);

		template <typename ComponentType, typename... ComponentTypes>
		bool HasAll(EntityId entity);

	public:
		template <typename ComponentType>
		const ComponentType& Get(EntityId entity) const;
		template <typename ComponentType>
		ComponentType& Get(EntityId entity);

		template <typename ComponentType>
		const ComponentType* GetPtr(EntityId entity) const;
		template <typename ComponentType>
		ComponentType* GetPtr(EntityId entity);

		template <typename... ComponentTypes>
		std::tuple<const ComponentTypes&...> GetAll(EntityId entity) const;
		template <typename... ComponentTypes>
		std::tuple<ComponentTypes&...> GetAll(EntityId entity);

	public:
		template<typename... ComponentTypes>
		BasicView<ComponentTypes...> View()
		{
			return BasicView<ComponentTypes...>(*this);
		}

	public:
		// Function = std::function<void(EntityId)>
		template<typename Function>
		void ForEachEntities(Function&& function);

		// Function = std::function<void(EntityId, ComponentType& component)>
		template<typename ComponentType, typename Function>
		void ForEachUniqueComponent(Function&& function);
		// Function = std::function<void(EntityId, ComponentType& component)>
		template<typename ComponentType, typename Function>
		void ForEachUniqueComponent(Function&& function) const;
		// Function = std::function<void(EntityId, ComponentTypes&... components)>
		template<typename... ComponentTypes, typename Function>
		void ForEachComponents(Function&& function);
		// Function = std::function<void(EntityId, ComponentTypes&... components)>
		template<typename... ComponentTypes, typename Function>
		void ForEachComponents(Function&& function) const;

	public:
		const auto& EachEntities();

		template<typename ComponentType>
		decltype(auto) EachEntitiesWith();
		template<typename ComponentType>
		decltype(auto) EachEntitiesWith() const;
		
		// TODO
		// template<typename ComponentType>
		// decltype(auto) EachUniqueComponent();
		// template<typename ComponentType>
		// decltype(auto) EachUniqueComponent() const;
	};
}

#include "Registry.inl"

#include "RegistryForEach.h"
#include "RegistryIterator.h"

#include "Views/BasicView.inl"
#include "Views/BasicViewForEach.h"
#include "Views/BasicViewIterator.h"
