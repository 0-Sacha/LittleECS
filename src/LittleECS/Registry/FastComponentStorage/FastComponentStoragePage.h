#pragma once

#include "../IComponentStorage.h"

#include <array>

namespace LECS::Detail
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

				DataStorageType() {}
				~DataStorageType() {}
            };
            DataStorageType Data{};
        };

    public:
        struct EntityLinkedWithoutRef
        {
            typename EntityId::Type Entity;

            inline bool constexpr IsValid() const
            {
                return Entity != EntityId::INVALID;
            }

            inline void constexpr SetInvalid()
            {
                Entity = EntityId::INVALID;
            }
        };

        struct EntityLinkedWithRef
        {
			typename EntityId::Type Entity;
            Index::IndexInAliveList IndexInAliveList;

			inline bool constexpr IsValid() const
			{
				return Entity != EntityId::INVALID;
			}

			inline void constexpr SetInvalid()
			{
				Entity = EntityId::INVALID;
			}
        };

        using EntityLinked = std::conditional_t<ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF, EntityLinkedWithRef, EntityLinkedWithoutRef>;

	private:
        template <typename... Args>
		inline ComponentType& ConstructAt(Index::PageIndexOfComponent index, Args&&... args)
		{
			ComponentDataBuffer* buffer = &m_Page[index];
			ComponentType* component = new (buffer) ComponentType(std::forward<Args>(args)...);
            return *component;
		}

        inline void DestroyAt(Index::PageIndexOfComponent index)
        {
            ComponentType& component = m_Page[index].Data.ComponentValue;
            component.~ComponentType();
        }

    public:
        FastComponentStoragePage()
        {
            for (std::size_t i = 0; i < PAGE_SIZE; ++i)
                m_EntitiesLinked[i].SetInvalid();
        }

		~FastComponentStoragePage()
		{
			// ForEach([this](Index::PageIndexOfComponent index) { this->DestroyAt(index); });
		}

    protected:
        std::array<ComponentDataBuffer, PAGE_SIZE> m_Page;
        EntityLinked m_EntitiesLinked[PAGE_SIZE];

    public:
    	inline typename EntityId::Type GetEntityIdAtIndex(Index::PageIndexOfComponent index) const
		{
			return m_EntitiesLinked[index].Entity;
        }

        inline bool HasEntityAtIndex(Index::PageIndexOfComponent index) const
		{
			return m_EntitiesLinked[index].IsValid();
        }

        inline Index::IndexInAliveList GetIndexInAliveListAtIndex(Index::PageIndexOfComponent index) const
		{
            if constexpr (ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF)
    			return m_EntitiesLinked[index].IndexInAliveList;
            else
                return 0;
        }

    public:
        inline void RemoveComponentAtIndex(Index::PageIndexOfComponent index)
        {
            LECS_ASSERT(HasEntityAtIndex(index) == true, "There are no component linked to this entity at this page")
            DestroyAt(index);
            m_EntitiesLinked[index].SetInvalid();
        }

    public:
        template<typename... Args>
        requires (ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF == true)
        ComponentType& AddComponent(EntityId entity, Index::PageIndexOfComponent index, Index::IndexInAliveList indexInAliveList, Args&&... args)
        {
            LECS_ASSERT(HasEntityAtIndex(index) == false, "Can't add this entity to this because it has the same id as another one")

            ComponentType& component = ConstructAt(index, std::forward<Args>(args)...);

            m_EntitiesLinked[index] = EntityLinked { .Entity = entity, .IndexInAliveList = indexInAliveList };

            return component;
        }

        template<typename... Args>
        requires (ComponentStorageInfo<ComponentType>::HAS_ENTITIES_REF == false)
        ComponentType& AddComponent(EntityId entity, Index::PageIndexOfComponent index, Args&&... args)
        {
            LECS_ASSERT(HasEntityAtIndex(index) == false, "Can't add this entity to this because it has the same id as another one")

            ComponentType& component = ConstructAt(index, std::forward<Args>(args)...);

            m_EntitiesLinked[index] = EntityLinked { .Entity = entity };

            return component;
        }

        ComponentType& GetComponentAtIndex(Index::PageIndexOfComponent index)
        {
			LECS_ASSERT(index < m_Page.size(), "There are no component linked to this entity at this page")
			LECS_ASSERT(HasEntityAtIndex(index) == true, "There are no component linked to this entity at this page")

            return *reinterpret_cast<ComponentType*>(&m_Page[index]);
		}

        const ComponentType& GetComponentAtIndex(Index::PageIndexOfComponent index) const
        {
			LECS_ASSERT(index < m_Page.size(), "There are no component linked to this entity at this page")
			LECS_ASSERT(HasEntityAtIndex(index) == true, "There are no component linked to this entity at this page")

            return *reinterpret_cast<ComponentType*>(&m_Page[index]);
		}

        ComponentType* GetComponentAtIndexPtr(Index::PageIndexOfComponent index)
		{
			if (index >= m_Page.size())
				return nullptr;
            if (HasEntityAtIndex(index) == false)
                return nullptr;
            return reinterpret_cast<ComponentType*>(&m_Page[index]);
		}

        const ComponentType* GetComponentAtIndexPtr(Index::PageIndexOfComponent index) const
        {
			if (index >= m_Page.size())
				return nullptr;
            if (HasEntityAtIndex(index) == false)
                return nullptr;
            return reinterpret_cast<const ComponentType*>(&m_Page[index]);
		}
    };
}
