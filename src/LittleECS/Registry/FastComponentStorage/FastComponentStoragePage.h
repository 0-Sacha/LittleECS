#pragma once

#include "../IComponentStorage.h"

#include <array>

namespace LittleECS::Detail
{
    template <typename ComponentType, std::size_t PAGE_SIZE>
    requires (PAGE_SIZE % sizeof(std::size_t) == 0)
    class FastComponentStoragePage
    {
    private:
        struct ComponentDataBuffer
        {
            union DataStorageType
            {
                std::uint8_t StorageData[sizeof(ComponentType)];
                ComponentType ComponentValue;
            };
            DataStorageType Data{};
        };

	private:
        template <typename... Args>
		inline ComponentType& ConstructAt(IComponentStorage::PageIndexOfComponent index, Args&&... args)
		{
			ComponentDataBuffer* buffer = &m_Page[index];
			ComponentType* component = new (buffer) ComponentType(std::forward<Args>(args)...);
            return *component;
		}

        inline void DestroyAt(IComponentStorage::PageIndexOfComponent index)
        {
            ComponentType& component = m_Page[index].Data.ComponentValue;
            component.~ComponentType();
        }

    public:
        FastComponentStoragePage()
        {
            for (std::size_t i = 0; i < PAGE_SIZE; ++i)
                m_EntityIdLinked[i] = EntityId::NON_VALID;
        }

		~FastComponentStoragePage()
		{
			// ForEach([this](IComponentStorage::PageIndexOfComponent index) { this->DestroyAt(index); });
		}

    protected:
        std::array<ComponentDataBuffer, PAGE_SIZE> m_Page;
        EntityId m_EntityIdLinked[PAGE_SIZE];

    public:
    	inline EntityId GetEntityIdAtIndex(IComponentStorage::PageIndexOfComponent index) const
		{
			return m_EntityIdLinked[index];
        }

        inline bool HasEntityAtIndex(IComponentStorage::PageIndexOfComponent index) const
		{
			return m_EntityIdLinked[index] != EntityId::NON_VALID;
        }

    public:
        template<typename... Args>
        ComponentType& AddComponent(EntityId entity, IComponentStorage::PageIndexOfComponent index, Args&&... args)
        {
            LECS_ASSERT(m_EntityIdLinked[index] == EntityId::NON_VALID, "Can't add this entity to this because it has the same id as another one");

            ComponentType& component = ConstructAt(index, std::forward<Args>(args)...);
            m_EntityIdLinked[index] = entity;
            return component;
        }

        void RemoveComponentAtIndex(IComponentStorage::PageIndexOfComponent index)
        {
            LECS_ASSERT(m_EntityIdLinked[index] != EntityId::NON_VALID, "There are no component linked to this entity at this page");

            DestroyAt(index);
            m_EntityIdLinked[index] = EntityId::NON_VALID;
        }

        ComponentType& GetComponentAtIndex(IComponentStorage::PageIndexOfComponent index)
        {
            LECS_ASSERT(m_EntityIdLinked[index] != EntityId::NON_VALID, "There are no component linked to this entity at this page");

            return *reinterpret_cast<ComponentType*>(&m_Page[index]);
		}

        const ComponentType& GetComponentAtIndex(IComponentStorage::PageIndexOfComponent index) const
        {
            LECS_ASSERT(m_EntityIdLinked[index] != EntityId::NON_VALID, "There are no component linked to this entity at this page");

            return *reinterpret_cast<ComponentType*>(&m_Page[index]);
		}
    };
}
 