#pragma once

#include "LittleECS/Detail/EntityIdGenerator.h"

#include "IComponentStorage.h"

#include "LittleECS/Entity/Entity.h"
#include "LittleECS/Entity/LiteEntity.h"

#include "Views/BasicView.h"

#include <unordered_map>
#include <memory>

namespace LECS
{
    class Registry
    {
    public:
        struct ComponentData
        {
            std::unique_ptr<Detail::IComponentStorage> ComponentStorage;
            std::function<void(EntityId)> OnConstruct;
            std::function<void(EntityId)> OnDestruct;
        };
        using ComponentIdToComponentData = std::unordered_map<ComponentId::Type, ComponentData>;
        using ComponentIdGenerator = Detail::CompilerComponentIdGenerator;

    public:
        Registry() {}

    protected:
        ComponentIdToComponentData m_ComponentIdToComponentData;
        Detail::EntityIdGenerator m_EntityIdGenerator;

    // Entity Management
    public:
        const Detail::EntityIdGenerator& GetEntityIdGenerator() const
        {
            return m_EntityIdGenerator;
        }
        const ComponentIdToComponentData& GetComponentIdToComponentData() const
        {
            return m_ComponentIdToComponentData;
        }

        bool RegistryHas(EntityId entity) const
        {
            return m_EntityIdGenerator.HasEntityId(entity);
        }
        
        EntityId CreateEntityId()
        {
            return m_EntityIdGenerator.GetNewEntityId();
        }

        void DestroyEntityId(EntityId entity)
        {
            for (auto& container : m_ComponentIdToComponentData)
            {
                if (container.second.ComponentStorage->HasThisComponentV(entity))
                {
                    container.second.ComponentStorage->RemoveComponentOfEntityV(entity);
                }
            }

            m_EntityIdGenerator.EntityIdDelete(entity);
        }

        Entity CreateEntityFrom(EntityId entity)
        {
            return Entity(this, entity);
        }

        LiteEntity CreateLiteEntityFrom(EntityId entity)
        {
            return LiteEntity(this, entity);
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
        const auto& EachEntities()
        {
            return m_EntityIdGenerator.GetAlivesEntities();
        }

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

#include "Registry-inl.h"
#include "RegistryForEach-inl.h"
#include "RegistryIterator-inl.h"

#include "Views/BasicView-inl-r.h"

#include "LittleECS/Entity/Entity-inl-r.h"
#include "LittleECS/Entity/LiteEntity-inl-r.h"