#pragma once

#include "Entity.h"

namespace LECS
{
    void ConstEntity::Refresh()
    {
     	for (auto& container : m_Registry.GetComponentIdToComponentData())
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
        ComponentId componentId = Registry::ComponentIdGenerator::GetTypeId<ComponentType>();

        const auto component = m_ComponentsContainer.find(componentId)

        if (component == m_ComponentsContainer.end())
            return nullptr;

        return reinterpret_cast<const ComponentType*>(component->second);
    }

    template <typename ComponentType>
	bool ConstEntity::Has() const
    {
        if constexpr (Detail::ComponentStorageInfo<ComponentTypeEach>::PTR_TO_COMPONENT_VALID)
        {
            return GetComponentPtr<ComponentType>() != nullptr;
        }
        else
        {
            return m_Registry.Has<ComponentType>(m_EntityId);
        }
    }

    template <typename ComponentType>
    const ComponentType& ConstEntity::Get() const
    {
        if constexpr (Detail::ComponentStorageInfo<ComponentTypeEach>::PTR_TO_COMPONENT_VALID)
        {
            return *GetComponentPtr<ComponentType>();
        }
        else
        {
            return m_Registry.Get<ComponentType>(m_EntityId);
        }
    }
    template <typename ComponentType>
    const ComponentType* ConstEntity::GetPtr() const
    {
        if constexpr (Detail::ComponentStorageInfo<ComponentTypeEach>::PTR_TO_COMPONENT_VALID)
        {
            return GetComponentPtr<ComponentType>();
        }
        else
        {
            return m_Registry.GetPtr<ComponentType>(m_EntityId);
        }
    }
    template <typename... ComponentTypes>
    std::tuple<const ComponentTypes&...> ConstEntity::GetAll() const
    {
        if constexpr (Detail::ComponentStorageInfo<ComponentTypeEach>::PTR_TO_COMPONENT_VALID)
        {
			return std::tuple<const ComponentTypes&...>(Get<ComponentTypes>(m_EntityId)...);
        }
        else
        {
            return m_Registry.GetAll<ComponentTypes...>(m_EntityId);
        }
    }
}

namespace LECS
{
    template <typename... ComponentTypes>
    std::tuple<ComponentTypes&...> Entity::GetAll()
    {
        if constexpr (Detail::ComponentStorageInfo<ComponentTypeEach>::PTR_TO_COMPONENT_VALID)
        {
			return std::tuple<ComponentTypes&...>(Get<ComponentTypes>(m_EntityId)...);
        }
        else
        {
            return m_Registry.GetAll<ComponentTypes...>(m_EntityId);
        }
    }

    template <typename ComponentType, typename... Args>
    ComponentType& Entity::Add(EntityId entity, Args&&... args)
    {
        ComponentId componentId = Registry::ComponentIdGenerator::GetTypeId<ComponentType>();

        ComponentType& res = m_Registry.Add<ComponentType>(m_EntityId, std::forward<Args>(args)...);
        m_ComponentsContainer.insert({ componentId, &res });
        return res;
    }
}
