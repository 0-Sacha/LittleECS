#pragma once

#include "LittleECS/Detail/ComponentId.h"
#include "LittleECS/Detail/EntityId.h"

#include "LittleECS/Detail/ComponentIdGenerator.h"

#include <any>

namespace LittleECS::Detail
{
    class IComponentStorage
    {
	public:
        using GlobalIndexOfComponent = std::size_t;
        using PageIndexOfComponent = std::size_t;
        using IndexOfPage = std::size_t;

        struct IndexInfo
        {
            IndexOfPage IndexOfPage;
            PageIndexOfComponent PageIndexOfComponent;

            inline bool IsValid()
            {
                return IndexOfPage != std::numeric_limits<std::size_t>::max();
            }

            inline bool SetInvalid()
            {
                return IndexOfPage = std::numeric_limits<std::size_t>::max();
            }
        };

    public:
        virtual ~IComponentStorage() = default;

	public:
		virtual bool EntityHasThisComponent(EntityId entity) const = 0;
		virtual void ForEach(std::any function) = 0;
		virtual void ForEach(std::any function) const = 0;
	};

    template <typename ComponentType>
    class FastComponentStorage;

	template <typename ComponentType>
    struct ComponentStorageInfo
    {
        using StorageType = FastComponentStorage<ComponentType>;
		static constexpr IComponentStorage::GlobalIndexOfComponent PAGE_SIZE = 1024;
    };

}
