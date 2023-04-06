#pragma once

#include "LittleECS/Registry/IComponentStorage.h"

#include <array>

namespace LittleECS
{
    class Registry;

    template <typename... ComponentTypes>
    class BasicView
    {

    public:
        BasicView(Registry& linkedRegistry)
            : m_LinkedRegistry(linkedRegistry)
            , m_LinkToComponentContainer{ nullptr }
        {
            CreateRegistryLink();
        }

    private:
        Registry& m_LinkedRegistry;
        std::array<Detail::IComponentStorage*, sizeof...(ComponentTypes)> m_LinkToComponentContainer;

    private:
        template <std::size_t I, typename Component, typename... ComponentRest>
        void CreateRegistryLinkImpl();

        void CreateRegistryLink()
        {
            return CreateRegistryLinkImpl<0, ComponentTypes...>();
        }

    public:
        void ForEach(std::function<void(EntityId, ComponentTypes&... components)> function)
        {

        }
    };
}
