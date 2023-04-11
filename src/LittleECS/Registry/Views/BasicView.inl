#pragma once

#include "BasicView.h"

namespace LittleECS
{
    template <typename... ViewComponentTypes>
    template <std::size_t I, typename Component, typename... ComponentRest>
    void BasicConstView<ViewComponentTypes...>::CreateRegistryLink()
    {
        m_LinkToComponentContainer[I] = m_LinkedRegistry.GetComponentStorage<Component>();
		LECS_ASSERT(m_LinkToComponentContainer[I] != nullptr, "Can't create a view with an unreferenced storage")
        if constexpr (sizeof...(ComponentRest) > 0)
            CreateRegistryLink<I + 1, ComponentRest...>();
    }
}
