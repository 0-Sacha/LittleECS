#pragma once

#include "Entity.h"

namespace LECS
{
    inline ConstEntity::ConstEntity()
        : m_Registry(nullptr)
        , m_EntityId(EntityId::INVALID)
        , m_ComponentsContainer()
    {}

    inline ConstEntity::ConstEntity(const Registry* registry, EntityId entityId)
        : m_Registry(registry)
        , m_EntityId(entityId)
        , m_ComponentsContainer()
    {
        LECS_ASSERT(m_EntityId != EntityId::INVALID)
        LECS_ASSERT(m_Registry != nullptr)
        LECS_ASSERT(m_Registry->RegistryHas(m_EntityId))

        Refresh();
    }

    inline void ConstEntity::Refresh()
    {
        LECS_ASSERT(IsValid())

         for (auto& container : m_Registry->GetComponentIdToComponentData())
        {
            const void* component = container.second.ComponentStorage->GetComponentAliasedPtrV(m_EntityId);
            if (component != nullptr)
            {
                m_ComponentsContainer.insert({ container.first, component });
            }
        }
    }

    template <typename ComponentType>
    const ComponentType* ConstEntity::GetComponentPtr() const
    {
        LECS_ASSERT(IsValid())

        ComponentId componentId = Registry::ComponentIdGenerator::GetTypeId<ComponentType>();

        const auto component = m_ComponentsContainer.find(componentId);

        if (component == m_ComponentsContainer.end())
            return nullptr;

        return reinterpret_cast<const ComponentType*>(component->second);
    }

    template <typename ComponentType>
    bool ConstEntity::Has() const
    {
        LECS_ASSERT(IsValid())

        if constexpr (Detail::ComponentStorageInfo<ComponentType>::PTR_TO_COMPONENT_VALID)
        {
            return GetComponentPtr<ComponentType>() != nullptr;
        }
        else
        {
            return m_Registry->Has<ComponentType>(m_EntityId);
        }
    }

    template <typename ComponentType>
    const ComponentType& ConstEntity::Get() const
    {
        LECS_ASSERT(IsValid())

        if constexpr (Detail::ComponentStorageInfo<ComponentType>::PTR_TO_COMPONENT_VALID)
        {
            return *GetComponentPtr<ComponentType>();
        }
        else
        {
            return m_Registry->Get<ComponentType>(m_EntityId);
        }
    }
    template <typename ComponentType>
    const ComponentType* ConstEntity::GetPtr() const
    {
        LECS_ASSERT(IsValid())

        if constexpr (Detail::ComponentStorageInfo<ComponentType>::PTR_TO_COMPONENT_VALID)
        {
            return GetComponentPtr<ComponentType>();
        }
        else
        {
            return m_Registry->GetPtr<ComponentType>(m_EntityId);
        }
    }
    template <typename... ComponentTypes>
    std::tuple<const ComponentTypes&...> ConstEntity::GetAll() const
    {
        LECS_ASSERT(IsValid())

        return std::tuple<const ComponentTypes&...>(Get<ComponentTypes>(m_EntityId)...);
    }
}

namespace LECS
{
    inline Entity::Entity()
        : ConstEntity()
    {}

    inline Entity::Entity(Registry* registry, EntityId entityId)
        : ConstEntity(registry, entityId)
    {}
    
    template <typename... ComponentTypes>
    std::tuple<ComponentTypes&...> Entity::GetAll()
    {
        LECS_ASSERT(IsValid())

        return std::tuple<ComponentTypes&...>(Get<ComponentTypes>(m_EntityId)...);
    }

    template <typename ComponentType, typename... Args>
    ComponentType& Entity::Add(Args&&... args)
    {
        LECS_ASSERT(IsValid())
        
        ComponentId componentId = Registry::ComponentIdGenerator::GetTypeId<ComponentType>();

        ComponentType& res = m_Registry->Add<ComponentType>(m_EntityId, std::forward<Args>(args)...);
        m_ComponentsContainer.insert({ componentId, &res });
        return res;
    }
}
