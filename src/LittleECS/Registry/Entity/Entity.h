#pragma once

#include "LittleECS/Detail/ComponentId.h"
#include "LittleECS/Detail/EntityId.h"

#include <unordered_map>

namespace LECS
{
	class Registry;

    class ConstEntity
    {
    public:
		using ComponentsContainer = std::unordered_map<ComponentId::Type, const void*>;
    
    public:
		ConstEntity();
		ConstEntity(const Registry* registry, EntityId entityId);
        
    protected:
        const Registry* m_Registry;
        EntityId m_EntityId;
        ComponentsContainer m_ComponentsContainer;
    
    public:
		bool IsValid() const
        {
            return m_EntityId != EntityId::INVALID && m_Registry != nullptr;
        }

		void Invalidate()
		{
			m_EntityId = EntityId::INVALID;
			m_Registry != nullptr;
		}

    public:
        void Refresh();

    protected:
        template <typename ComponentType>
        const ComponentType* GetComponentPtr() const;
        
    public:
        inline EntityId GetEntityId() { return m_EntityId; }
        inline operator EntityId () { return m_EntityId; }
        inline operator bool () { return m_EntityId != EntityId::INVALID; }

    public:
        template <typename ComponentType>
		bool Has() const;

        template <typename ComponentType>
		const ComponentType& Get() const;
		template <typename ComponentType>
		const ComponentType* GetPtr() const;
		template <typename... ComponentTypes>
		std::tuple<const ComponentTypes&...> GetAll() const;
    };

    class Entity : public ConstEntity
    {
    public:
		using ComponentsContainer = std::unordered_map<ComponentId::Type, const void*>;
    
    public:
        Entity();
        Entity(Registry* registry, EntityId entityId);
        
    protected:
        template <typename ComponentType>
        const ComponentType* GetComponentPtr() const
        {
            return ConstEntity::template GetComponentPtr<ComponentType>();
        }

        template <typename ComponentType>
        ComponentType* GetComponentPtr()
        {
            return const_cast<ComponentType*>(ConstEntity::template GetComponentPtr<ComponentType>());
        }
        

    public:
        template <typename ComponentType>
		bool Has() const
        {
            return ConstEntity::template Has<ComponentType>();
        }

        template <typename ComponentType>
		const ComponentType& Get() const
        {
            return ConstEntity::template Get<ComponentType>();
        }
        template <typename ComponentType>
		ComponentType& Get()
        {
            return const_cast<ComponentType&>(ConstEntity::template Get<ComponentType>());
        }

		template <typename ComponentType>
		const ComponentType* GetPtr() const
        {
            return ConstEntity::template GetPtr<ComponentType>();
        }
        template <typename ComponentType>
		ComponentType* GetPtr()
        {
            return const_cast<ComponentType*>(ConstEntity::template GetPtr<ComponentType>());
        }

		template <typename... ComponentTypes>
		std::tuple<const ComponentTypes&...> GetAll() const
        {
            return ConstEntity::template GetAll<ComponentTypes...>();
        }
        template <typename... ComponentTypes>
		std::tuple<ComponentTypes&...> GetAll();

    public:
        template <typename ComponentType, typename... Args>
		ComponentType& Add(Args&&... args);
    };           
}
