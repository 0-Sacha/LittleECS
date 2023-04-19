#pragma once

#include "LittleECS/Detail/ComponentId.h"
#include "LittleECS/Detail/EntityId.h"

namespace LECS
{
    class Registry;
    class LiteEntity
    {
    public:
		LiteEntity();
		LiteEntity(Registry* registry, EntityId entityId);
        
    protected:
        Registry* m_Registry;
        EntityId m_EntityId;

    public:
        inline EntityId GetEntityId() { return m_EntityId; }
        inline operator EntityId () { return m_EntityId; }
        inline operator bool () { return m_EntityId != EntityId::INVALID; }

    public:
		bool IsValid() const
        {
            return m_EntityId != EntityId::INVALID && m_Registry != nullptr;
        }

    public:
        template <typename ComponentType>
		bool Has() const;
        template <typename ComponentType>
		const ComponentType& Get() const;
        template <typename ComponentType>
		ComponentType& Get();
		template <typename... ComponentTypes>
		std::tuple<const ComponentTypes&...> GetAll() const;
        template <typename... ComponentTypes>
		std::tuple<ComponentTypes&...> GetAll();
        template <typename ComponentType, typename... Args>
		ComponentType& Add(Args&&... args);
    };
}