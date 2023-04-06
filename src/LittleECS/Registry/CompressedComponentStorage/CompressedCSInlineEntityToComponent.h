#pragma once

#include "CompressedComponentStoragePage.h"

namespace LittleECS::Detail 
{

    template <bool HAS_ENTITIES_REF, std::size_t PAGE_SIZE>
    class CompressedCSInlineEntityToComponent
    {
    public:
        struct BucketElementWithoutRef
        {
            Index::IndexInfo IndexInfo;
        };

        struct BucketElementWithRef
        {
            Index::IndexInfo IndexInfo;
            Index::IndexInAliveList IndexInAliveList;
        };

        using BucketElement = std::conditional_t<HAS_ENTITIES_REF, BucketElementWithRef, BucketElementWithoutRef>;

        using Bucket = std::array<BucketElement, PAGE_SIZE>;
        using BucketRef = std::unique_ptr<Bucket>;
        using BucketContainer = std::vector<BucketRef>;
        using BucketIndexInfo = Index::IndexInfo;
        using AliveEntitiesContainerType = std::vector<typename EntityId::Type>;
        using AliveEntitiesContainer = std::conditional_t<HAS_ENTITIES_REF, AliveEntitiesContainerType, int>;

    public:
        CompressedCSInlineEntityToComponent()
        {
            if constexpr (HAS_ENTITIES_REF)
                m_AliveEntitiesContainer.reserve(PAGE_SIZE);
        }

    private:
        BucketContainer m_BucketContainer;
        AliveEntitiesContainer m_AliveEntitiesContainer;

    private:
        inline BucketIndexInfo GetBucketInfoOfEntity(EntityId entity) const
        {
            BucketIndexInfo bucketIndexInfo;
            bucketIndexInfo.IndexOfPage = entity.Id / PAGE_SIZE;
            bucketIndexInfo.PageIndexOfComponent = entity.Id % PAGE_SIZE;
            return bucketIndexInfo;
        }

    public:
        const AliveEntitiesContainer& GetAliveContainer()
        {
            return m_AliveEntitiesContainer;
        }

    public:
        inline bool HasEntity(EntityId entity) const
        {
            BucketIndexInfo bucketInfo = GetBucketInfoOfEntity(entity);

            if (bucketInfo.IndexOfPage >= m_BucketContainer.size())
                return false;

            const BucketRef& bucketRef = m_BucketContainer[bucketInfo.IndexOfPage];

            if (bucketRef == nullptr)
                return false;

            Index::IndexInfo indexInfo = (*bucketRef)[bucketInfo.PageIndexOfComponent].IndexInfo;

            return indexInfo.IsValid();
        }

        inline Index::IndexInfo GetIndexInfoOfEntity(EntityId entity) const
        {
            BucketIndexInfo bucketInfo = GetBucketInfoOfEntity(entity);
            
            LECS_ASSERT(bucketInfo.IndexOfPage < m_BucketContainer.size(), "This container can have this entity");

            const BucketRef& bucketRef = m_BucketContainer[bucketInfo.IndexOfPage];

            LECS_ASSERT(bucketRef != nullptr, "This container can have this entity");

            return (*bucketRef)[bucketInfo.PageIndexOfComponent].IndexInfo;
        }

        inline void AddIndexInfoForEntity(EntityId entity, Index::IndexInfo indexInfo)
        {
            BucketIndexInfo bucketInfo = GetBucketInfoOfEntity(entity);

            if (bucketInfo.IndexOfPage >= m_BucketContainer.size())
            {
                m_BucketContainer.resize(bucketInfo.IndexOfPage + 1);
            }

            BucketRef& bucketRef = m_BucketContainer[bucketInfo.IndexOfPage];

            if (bucketRef == nullptr)
            {
                bucketRef = std::make_unique<Bucket>();
                Bucket& bucket = *bucketRef;
                for (std::size_t i = 0; i < PAGE_SIZE; ++i)
                    bucket[i].IndexInfo.SetInvalid();
            }

            if constexpr (HAS_ENTITIES_REF)
            {
                m_AliveEntitiesContainer.emplace_back(entity.Id);
                (*bucketRef)[bucketInfo.PageIndexOfComponent] = BucketElement { .IndexInfo = indexInfo, .IndexInAliveList = m_AliveEntitiesContainer.size() - 1 };
            }
            else
            {
                (*bucketRef)[bucketInfo.PageIndexOfComponent] = BucketElement { .IndexInfo = indexInfo };
            }
        }

        inline void RemoveIndexInfoForEntity(EntityId entity)
        {
            BucketIndexInfo bucketInfo = GetBucketInfoOfEntity(entity);

            LECS_ASSERT(bucketInfo.IndexOfPage < m_BucketContainer.size(), "This container can have this entity");

            BucketRef& bucketRef = m_BucketContainer[bucketInfo.IndexOfPage];

            LECS_ASSERT(bucketRef != nullptr, "This container can have this entity");

            if constexpr (HAS_ENTITIES_REF)
            {
              Index::IndexInAliveList indexInAliveList = (*bucketRef)[bucketInfo.PageIndexOfComponent].IndexInAliveList;
                typename EntityId::Type lastEntity = m_AliveEntitiesContainer.back();
                m_AliveEntitiesContainer[indexInAliveList] = lastEntity;
                m_AliveEntitiesContainer.pop_back();
            }
            
            (*bucketRef)[bucketInfo.PageIndexOfComponent].IndexInfo.SetInvalid();
        }
    };

}
