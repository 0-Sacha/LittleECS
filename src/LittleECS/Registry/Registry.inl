#pragma once

#include "Registry.h"

namespace LECS
{
	template <typename ComponentType>
	typename Detail::ComponentStorageInfo<ComponentType>::StorageType* Registry::GetComponentStorageOrCreateIt()
	{
		ComponentId componentId = ComponentIdGenerator::GetTypeId<ComponentType>();

		if (m_ComponentsStoragesContainer.contains(componentId) == true)
		{
			Detail::IComponentStorage* componentStorageBasic = m_ComponentsStoragesContainer[componentId].get();
			return reinterpret_cast<typename Detail::ComponentStorageInfo<ComponentType>::StorageType*>(componentStorageBasic);
		}
		else
		{
			ComponentStorageRef& componentStorageRef = m_ComponentsStoragesContainer[componentId] = std::make_unique<typename Detail::ComponentStorageInfo<ComponentType>::StorageType>();
			Detail::IComponentStorage* componentStorageBasic = componentStorageRef.get();
			return reinterpret_cast<typename Detail::ComponentStorageInfo<ComponentType>::StorageType*>(componentStorageBasic);
		}

		return nullptr;
	}

	template <typename ComponentType>
	const typename Detail::ComponentStorageInfo<ComponentType>::StorageType* Registry::GetComponentStorage() const
	{
		ComponentId componentId = ComponentIdGenerator::GetTypeId<ComponentType>();

		const auto componentStorage = m_ComponentsStoragesContainer.find(componentId);

		LECS_ASSERT(componentStorage != m_ComponentsStoragesContainer.end(), "This ComponentStorage is not part of this registry")

		const Detail::IComponentStorage* componentStorageBasic = componentStorage->second.get();
		return reinterpret_cast<const typename Detail::ComponentStorageInfo<ComponentType>::StorageType*>(componentStorageBasic);
	}

	template <typename ComponentType, typename... Args>
	ComponentType& Registry::Add(EntityId entity, Args&&... args)
	{
		typename Detail::ComponentStorageInfo<ComponentType>::StorageType* componentStorage = GetComponentStorageOrCreateIt<ComponentType>();
		LECS_ASSERT(componentStorage != nullptr, "This ComponentStorage is not part of this registry")
		return componentStorage->AddComponentToEntity(entity, std::forward<Args>(args)...);
	}

	template <typename ComponentType>
	bool Registry::Has(EntityId entity)
	{
		ComponentId componentId = ComponentIdGenerator::GetTypeId<ComponentType>();

		auto componentStoragefound = m_ComponentsStoragesContainer.find(componentId);

		if (componentStoragefound == m_ComponentsStoragesContainer.end())
			return false;

		Detail::IComponentStorage* componentStorageBasic = componentStoragefound->second.get();
		typename Detail::ComponentStorageInfo<ComponentType>::StorageType* componentStorage = reinterpret_cast<typename Detail::ComponentStorageInfo<ComponentType>::StorageType*>(componentStorageBasic);

		if (componentStorage == nullptr)
			return false;

		return componentStorage->HasThisComponent(entity);
	}

	template <typename ComponentType, typename... ComponentTypes>
	bool Registry::HasAll(EntityId entity)
	{
		if constexpr (sizeof...(ComponentTypes) == 0)
			return HasComponent<ComponentType>(entity);
		else
			return HasComponent<ComponentType>(entity) && HasAllComponents<ComponentTypes...>(entity);
	}

	template <typename ComponentType>
	const ComponentType& Registry::Get(EntityId entity) const
	{
		const typename Detail::ComponentStorageInfo<ComponentType>::StorageType* componentStorage = GetComponentStorage<ComponentType>();
		LECS_ASSERT(componentStorage != nullptr, "This ComponentStorage is not part of this registry")
		return componentStorage->GetComponentOfEntity(entity);
	}
	template <typename ComponentType>
	ComponentType& Registry::Get(EntityId entity)
	{
		typename Detail::ComponentStorageInfo<ComponentType>::StorageType* componentStorage = GetComponentStorage<ComponentType>();
		LECS_ASSERT(componentStorage != nullptr, "This ComponentStorage is not part of this registry")
		return componentStorage->GetComponentOfEntity(entity);
	}

	template <typename ComponentType>
	const ComponentType* Registry::GetPtr(EntityId entity) const
	{
		const typename Detail::ComponentStorageInfo<ComponentType>::StorageType* componentStorage = GetComponentStorage<ComponentType>();
		if (componentStorage == nullptr)
			return nullptr;
		return componentStorage->GetComponentOfEntityPtr(entity);
	}
	template <typename ComponentType>
	ComponentType* Registry::GetPtr(EntityId entity)
	{
		typename Detail::ComponentStorageInfo<ComponentType>::StorageType* componentStorage = GetComponentStorage<ComponentType>();
		if (componentStorage == nullptr)
			return nullptr;
		return componentStorage->GetComponentOfEntityPtr(entity);
	}

	template <typename... ComponentTypes>
	std::tuple<const ComponentTypes&...> Registry::GetAll(EntityId entity) const
	{
		return std::tuple<const ComponentTypes&...>(Get<ComponentTypes>(entity)...);
	}
	template <typename... ComponentTypes>
	std::tuple<ComponentTypes&...> Registry::GetAll(EntityId entity)
	{
		return std::tuple<ComponentTypes&...>(Get<ComponentTypes>(entity)...);
	}

	// Function = std::function<void(EntityId)>
	template<typename Function>
	void Registry::ForEachEntities(Function&& function)
	{
		for (EntityId entity : m_EntityIdGenerator.GetAlivesEntities())
			function(entity);
	}

	const auto& Registry::EachEntities()
	{
		return m_EntityIdGenerator.GetAlivesEntities();
	}
}