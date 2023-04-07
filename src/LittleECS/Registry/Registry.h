#pragma once

#include "LittleECS/Detail/EntityIdGenerator.h"

#include "CompressedComponentStorage/CompressedComponentStorage.h"
#include "FastComponentStorage/FastComponentStorage.h"
#include "Views/BasicView.h"

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

        void DestroyEntity(EntityId entity)
        {
            for (auto& container : m_ComponentsStoragesContainer)
            {
                if (container.second->EntityHasThisComponent(entity))
                {
                    container.second->RemoveComponentOfEntity(entity);
                }
            }

            m_EntityIdGenerator.EntityIdDelete(entity);
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

    public:
        template <typename ComponentType>
        typename Detail::ComponentStorageInfo<ComponentType>::StorageType* GetComponentStorage()
        {
            ComponentId componentId = ComponentIdGenerator::GetTypeId<ComponentType>();

            auto componentStorage = m_ComponentsStoragesContainer.find(componentId);

            LECS_ASSERT(componentStorage != m_ComponentsStoragesContainer.end(), "This ComponentStorage is not part of this registry");

            Detail::IComponentStorage* componentStorageBasic = componentStorage->second.get();
            return reinterpret_cast<typename Detail::ComponentStorageInfo<ComponentType>::StorageType*>(componentStorageBasic);
        }

        template <typename ComponentType>
        const typename Detail::ComponentStorageInfo<ComponentType>::StorageType* GetComponentStorage() const
        {
            ComponentId componentId = ComponentIdGenerator::GetTypeId<ComponentType>();

            const auto componentStorage = m_ComponentsStoragesContainer.find(componentId);

            LECS_ASSERT(componentStorage != m_ComponentsStoragesContainer.end(), "This ComponentStorage is not part of this registry");

            const Detail::IComponentStorage* componentStorageBasic = componentStorage->second.get();
            return reinterpret_cast<const typename Detail::ComponentStorageInfo<ComponentType>::StorageType*>(componentStorageBasic);
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

        // Function = std::function<void(EntityId, RangeComponent& component, ComponentTypes&... components)>
        template<typename RangeComponent, typename... ComponentTypes, typename Function>
        void ForEachComponents(Function&& function);

        // Function = std::function<void(EntityId, RangeComponent& component, ComponentTypes&... components)>
        template<typename RangeComponent, typename... ComponentTypes, typename Function>
        void ForEachComponents(Function&& function) const;
    };

}

#include "RegistryEach.h"

#include "Views/BasicView.inl"
