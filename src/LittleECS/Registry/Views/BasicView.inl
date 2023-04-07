#pragma once

#include "BasicView.h"

namespace LittleECS
{
    template <typename... ComponentTypes>
    template <std::size_t I, typename Component, typename... ComponentRest>
    void BasicView<ComponentTypes...>::CreateRegistryLinkImpl()
    {
        m_LinkToComponentContainer[I] = m_LinkedRegistry.GetComponentStorage<Component>();
        if constexpr (I > 0)
            CreateRegistryLinkImpl<I + 1, ComponentRest...>();
    }
}
