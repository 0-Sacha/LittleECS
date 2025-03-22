#pragma once

#include "lecs/detail/componentid.h"
#include "lecs/detail/entityid.h"

#include <unordered_map>

namespace lecs
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
        const Registry* registry_;
        EntityId entityid_;
        ComponentsContainer components_container;
    
    public:
        bool is_valid() const
        {
            return entityid_ != EntityId::INVALID && registry_ != nullptr;
        }

        void Invalidate()
        {
            entityid_ = EntityId::INVALID;
            registry_ = nullptr;
        }

    public:
        void refresh();

    protected:
        template <typename ComponentType>
        const ComponentType* GetComponentPtr() const;
        
    public:
        inline EntityId GetEntityId() { return entityid_; }
        inline operator EntityId () { return entityid_; }
        inline operator bool () { return entityid_ != EntityId::INVALID; }

    public:
        template <typename ComponentType>
        bool has() const;

        template <typename ComponentType>
        const ComponentType& get() const;
        template <typename ComponentType>
        const ComponentType* GetPtr() const;
        template <typename... ComponentTypes>
        std::tuple<const ComponentTypes&...> get_all() const;
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
        bool has() const
        {
            return ConstEntity::template has<ComponentType>();
        }

        template <typename ComponentType>
        const ComponentType& get() const
        {
            return ConstEntity::template get<ComponentType>();
        }
        template <typename ComponentType>
        ComponentType& get()
        {
            return const_cast<ComponentType&>(ConstEntity::template get<ComponentType>());
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
        std::tuple<const ComponentTypes&...> get_all() const
        {
            return ConstEntity::template get_all<ComponentTypes...>();
        }
        template <typename... ComponentTypes>
        std::tuple<ComponentTypes&...> get_all();

    public:
        template <typename ComponentType, typename... Args>
        ComponentType& Add(Args&&... args);
    };           
}
