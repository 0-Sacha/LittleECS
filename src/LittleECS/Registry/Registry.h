#pragma once

#include "LittleECS/Detail/EntityIdGenerator.h"

#include "CompressedComponentStorage/CompressedComponentStorage.h"
#include "FastComponentStorage/FastComponentStorage.h"

#include <unordered_map>
#include <memory>

namespace LittleECS
{

    class Registry
    {
    public:
        using ComponentStorageRef = std::unique_ptr<Detail::IComponentStorage>;
        using ComponentsStoragesContainer = std::unordered_map<ComponentId::Type, ComponentStorageRef>;
        using ComponentIdGenerator = Detail::GlobalComponentIdGenerator;

    protected:
        ComponentsStoragesContainer m_ComponentsStoragesContainer;
        Detail::EntityIdGenerator m_EntityIdGenerator;

    public:
        Registry() {}

    // Entity Management
    public:
        EntityId CreateEntity()
        {
            return m_EntityIdGenerator.GetNewEntityId();
        }

    // Entity's Components Management
    private:
        template <typename ComponentType>
        typename Detail::ComponentStorageInfo<ComponentType>::StorageType* GetComponentStorageOrCreateIt()
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
        typename Detail::ComponentStorageInfo<ComponentType>::StorageType* GetComponentStorage()
        {
            ComponentId componentId = ComponentIdGenerator::GetTypeId<ComponentType>();

            auto componentStorage = m_ComponentsStoragesContainer.find(componentId);

            LECS_ASSERT(m_ComponentsStoragesContainer.contains(componentId), "This ComponentStorage is not part of this registry");

            Detail::IComponentStorage* componentStorageBasic = m_ComponentsStoragesContainer[componentId].get();
            return reinterpret_cast<typename Detail::ComponentStorageInfo<ComponentType>::StorageType*>(componentStorageBasic);
        }

    public:
        template <typename ComponentType, typename... Args>
        ComponentType& AddComponentToEntity(EntityId entity, Args&&... args)
        {
            typename Detail::ComponentStorageInfo<ComponentType>::StorageType* componentStorage = GetComponentStorageOrCreateIt<ComponentType>();
            LECS_ASSERT(componentStorage != nullptr, "This ComponentStorage is not part of this registry");
            return componentStorage->AddComponentToEntity(entity, std::forward<Args>(args)...);
        }

        template <typename ComponentType>
        ComponentType& GetComponentOfEntity(EntityId entity)
        {
            typename Detail::ComponentStorageInfo<ComponentType>::StorageType* componentStorage = GetComponentStorage<ComponentType>();
            LECS_ASSERT(componentStorage != nullptr, "This ComponentStorage is not part of this registry");
            return componentStorage->GetComponentOfEntity(entity);
        }

        template <typename ComponentType>
        bool EntityHasComponent(EntityId entity)
        {
            ComponentId componentId = ComponentIdGenerator::GetTypeId<ComponentType>();

            auto componentStoragefound = m_ComponentsStoragesContainer.find(componentId);

            if (componentStoragefound == m_ComponentsStoragesContainer.end())
                return false;

            Detail::IComponentStorage* componentStorageBasic = componentStoragefound->second.get();
            typename Detail::ComponentStorageInfo<ComponentType>::StorageType* componentStorage = reinterpret_cast<typename Detail::ComponentStorageInfo<ComponentType>::StorageType*>(componentStorageBasic);

            if (componentStorage == nullptr)
                return false;

            return componentStorage->EntityHasThisComponent(entity);
        }
    };

}
