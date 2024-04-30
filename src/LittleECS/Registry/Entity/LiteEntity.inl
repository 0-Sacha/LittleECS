#pragma once

#include "LiteEntity.h"

namespace LECS
{
	inline LiteEntity::LiteEntity()
		: m_Registry(nullptr)
		, m_EntityId(EntityId::INVALID)
	{
	}

	inline LiteEntity::LiteEntity(Registry* registry, EntityId entityId)
		: m_Registry(registry)
		, m_EntityId(entityId)
	{
		LECS_ASSERT(m_EntityId != EntityId::INVALID)
		LECS_ASSERT(m_Registry != nullptr)
		LECS_ASSERT(m_Registry->RegistryHas(m_EntityId))
	}

    template <typename ComponentType>
	bool LiteEntity::Has() const
    {
		LECS_ASSERT(IsValid())

        return m_Registry->Has<ComponentType>(m_EntityId);
    }

    template <typename ComponentType>
    const ComponentType& LiteEntity::Get() const
    {
		LECS_ASSERT(IsValid())
        
        return m_Registry->Get<ComponentType>(m_EntityId);
    }

    template <typename ComponentType>
    ComponentType& LiteEntity::Get()
    {
		LECS_ASSERT(IsValid())

        return m_Registry->Get<ComponentType>(m_EntityId);
    }

    template <typename... ComponentTypes>
    std::tuple<const ComponentTypes&...> LiteEntity::GetAll() const
    {
		LECS_ASSERT(IsValid())

        return m_Registry->GetAll<ComponentTypes...>(m_EntityId);
    }

    template <typename... ComponentTypes>
    std::tuple<ComponentTypes&...> LiteEntity::GetAll()
    {
		LECS_ASSERT(IsValid())
        
        return m_Registry->GetAll<ComponentTypes...>(m_EntityId);
    }

    template <typename ComponentType, typename... Args>
	ComponentType& LiteEntity::Add(Args&&... args)
    {
		LECS_ASSERT(IsValid())
        
        return m_Registry->Add<ComponentType>(m_EntityId, std::forward<Args>(args)...);
    }
}
