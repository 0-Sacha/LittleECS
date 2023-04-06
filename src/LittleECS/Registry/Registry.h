#pragma once

#include "LittleECS/Detail/EntityIdGenerator.h"

#include "CompressedComponentStorage/CompressedComponentStorage.h"
#include "FastComponentStorage/FastComponentStorage.h"
#include "BasicView.h"

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

    public:
        template<typename... ComponentTypes>
        BasicView<ComponentTypes...> View()
        {
            return BasicView<ComponentTypes...>(*this);
        }

        template<typename ComponentType>
        void ForEachUniqueComponent(std::function<void(EntityId, ComponentType& component)> function)
        {
            typename Detail::ComponentStorageInfo<ComponentType>::StorageType* componentStorage = GetComponentStorage<ComponentType>();
            if (componentStorage == nullptr)
                return;
            
            if constexpr (Detail::ComponentStorageInfo<ComponentType>::SEND_ENTITIES_POOL_ON_EACH == false)
            {
                componentStorage->ForEachUniqueComponent(function);
            }
            else
            {
                componentStorage->ForEachUniqueComponent(function, m_EntityIdGenerator.GetAlivesEntities());
            }
        }

        template<typename RangeComponent, typename... ComponentTypes>
        void ForEach(std::function<void(EntityId, RangeComponent& component, ComponentTypes&... components)> function)
        {
            if constexpr (sizeof...(ComponentTypes) == 0)
            {
                return ForEachUniqueComponent<RangeComponent>(function);
            }
            else
            {
                BasicView<RangeComponent, ComponentTypes...> view(*this);
                view.ForEach(function);
            }
        }

        template<typename RangeComponent, typename... ComponentTypes, typename Function>
        void ForEach(Function&& function)
        {
            std::function<void(EntityId, RangeComponent& component, ComponentTypes&... components)> functionCast = function;
            return ForEach<RangeComponent, ComponentTypes...>(functionCast);
        }
    };

}

#include "BasicView.inl"
