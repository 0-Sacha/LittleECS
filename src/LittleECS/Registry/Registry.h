#pragma once

#include "ComponentStorage.h"
#include "LittleECS/Detail/EntityIdGenerator.h"

#include <map>
#include <memory>

namespace LittleECS
{

    class Registry
    {
    public:
        using ComponentStorageRef = std::unique_ptr<Detail::BasicComponentStorage>;
        using ComponentsStoragesContainer = std::map<ComponentId::Type, ComponentStorageRef>;
        using ComponentIdGenerator = Detail::GlobalComponentIdGenerator;

    // Entity Management
    public:
        EntityId CreateEntity()
        {
            return m_EntityIdGenerator.GetNewEntityId();
        }

    // Entity's Components Management
    public:
        template <typename ComponentType>
        Detail::ComponentStorage<ComponentType>* GetComponentStorageOrCreateIt()
        {
            ComponentId componentId = ComponentIdGenerator::GetTypeId<ComponentType>();

            // TODO : fix this crap
            if (m_ComponentsStoragesContainer.contains(componentId))
            {
                ComponentStorageRef& componentStorageRef = m_ComponentsStoragesContainer[componentId];
                Detail::BasicComponentStorage* componentStorageBasic = componentStorageRef.get();
                return reinterpret_cast<Detail::ComponentStorage<ComponentType>*>(componentStorageBasic);
            }
            else
            {
                ComponentStorageRef& componentStorageRef = m_ComponentsStoragesContainer[componentId] = std::make_unique<Detail::ComponentStorage<ComponentType>>();
                Detail::BasicComponentStorage* componentStorageBasic = componentStorageRef.get();
                return reinterpret_cast<Detail::ComponentStorage<ComponentType>*>(componentStorageBasic);
            }

            return nullptr;
        }

        template <typename ComponentType, typename... Args>
        ComponentType& AddComponentToEntity(EntityId entity, Args&&... args)
        {
            Detail::ComponentStorage<ComponentType>* componentStorage = GetComponentStorageOrCreateIt<ComponentType>();
            LECS_ASSERT(componentStorage != nullptr, "Got a nullptr ComponentStorage");
            return componentStorage->AddComponentToEntity(entity, std::forward<Args>(args)...);
        }

        template <typename ComponentType>
        ComponentType& GetComponentOfEntity(EntityId entity)
        {
            // TODO : must not create the container
            Detail::ComponentStorage<ComponentType>* componentStorage = GetComponentStorageOrCreateIt<ComponentType>();
            LECS_ASSERT(componentStorage != nullptr, "Got a nullptr ComponentStorage");
            return componentStorage->GetComponentOfEntity(entity);
        }

        template <typename ComponentType>
        bool EntityHasComponent(EntityId entity)
        {
            // TODO : must not create the container
            Detail::ComponentStorage<ComponentType>* componentStorage = GetComponentStorageOrCreateIt<ComponentType>();
            LECS_ASSERT(componentStorage != nullptr, "Got a nullptr ComponentStorage");

            return componentStorage->EntityHasThisComponent(entity);
        }

    protected:
        ComponentsStoragesContainer m_ComponentsStoragesContainer;
        Detail::EntityIdGenerator m_EntityIdGenerator;
    };

}
