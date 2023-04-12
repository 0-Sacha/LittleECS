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
        ConstEntity(const Registry& registry)
            : m_Registry(registry)
            , m_ComponentsContainer()
        {
            Refresh();
        }
        
    protected:
        const Registry& m_Registry;
        ComponentsContainer m_ComponentsContainer;
        EntityId m_EntityId;

    public:
        void Refresh();

    protected:
        template <typename ComponentType>
        const ComponentType* GetComponentPtr() const;
        
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
        Entity(Registry& registry)
            : ConstEntity(registry)
        {}
        
    protected:
        Registry& GetRegistry()
        {
            return const_cast<Registry&>(m_Registry);
        }

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
            return ConstEntity::template GetAll<ComponentType>();
        }
        template <typename... ComponentTypes>
		std::tuple<ComponentTypes&...> GetAll();

    public:
        template <typename ComponentType, typename... Args>
		ComponentType& Add(EntityId entity, Args&&... args);
    };

           
}
