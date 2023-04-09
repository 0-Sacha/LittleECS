#pragma once

#include "BasicView.h"

namespace LittleECS
{
    template <typename... ViewComponentTypes>
    template <std::size_t I, typename Component, typename... ComponentRest>
    void BasicView<ViewComponentTypes...>::CreateRegistryLink()
    {
        m_LinkToComponentContainer[I] = m_LinkedRegistry.GetComponentStorage<Component>();
        if constexpr (I > 0)
            CreateRegistryLink<I + 1, ComponentRest...>();
    }

    template <typename... ViewComponentTypes>
    template <std::size_t I, typename Component, typename... ComponentRest>
    void BasicConstView<ViewComponentTypes...>::CreateRegistryLink()
    {
        m_LinkToComponentContainer[I] = m_LinkedRegistry.GetComponentStorage<Component>();
        if constexpr (I > 0)
            CreateRegistryLink<I + 1, ComponentRest...>();
    }
}
