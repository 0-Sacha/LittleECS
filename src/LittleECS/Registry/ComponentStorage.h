#pragma once

#include "LittleECS/Detail/ComponentId.h"
#include "LittleECS/Detail/EntityId.h"

#include "LittleECS/Detail/ComponentIdGenerator.h"

#include <map>
#include <queue>
#include <deque>

namespace LittleECS::Detail
{
    class BasicComponentStorage
    {
    public:
        using IntexOfComponent = std::size_t;
        using EntityToComponent = std::map<typename EntityId::Type, IntexOfComponent>;
        using FreeComponentsList = std::queue<IntexOfComponent>;

    public:
        bool EntityHasThisComponent(EntityId entity)
        {
            return m_EntityToComponent.contains(entity.Id);
        }

    protected:
        EntityToComponent m_EntityToComponent;
        FreeComponentsList m_FreeComponents;
    };

    template <typename ComponentType>
    class ComponentStorage : public BasicComponentStorage
    {
    public:
        using ComponentsContainer = std::vector<ComponentType>;

    public:
        template <typename... Args>
        ComponentType& AddComponentToEntity(EntityId entity, Args&&... args)
        {
            LECS_ASSERT(EntityHasThisComponent(entity) == false);
            
            ComponentType& res = m_ComponentsContainer.emplace_back(std::forward<Args>(args)...);
            m_EntityToComponent[entity] = m_ComponentsContainer.size() - 1;
            return res;
        }

        ComponentType& GetComponentOfEntity(EntityId entity)
        {
            LECS_ASSERT(EntityHasThisComponent(entity));
            
            IntexOfComponent indexToComponent = m_EntityToComponent[entity];
            return m_ComponentsContainer[indexToComponent];
        }

    protected:
        ComponentsContainer m_ComponentsContainer;
    };
}
 