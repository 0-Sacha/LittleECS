#pragma once

#include "ccs_page.h"

namespace lecs::detail 
{
    template <bool HAS_ENTITIES_REF, std::size_t PAGE_SIZE>
    class CCS_EntityToComponent_Inline
    {
    public:
        struct BucketElementWithoutRef
        {
            Index::IndexInfo indexinfo;
        };

        struct BucketElementWithRef
        {
            Index::IndexInfo indexinfo;
            Index::index_in_alive_list index_in_alive_list;
        };

        using BucketElement = std::conditional_t<HAS_ENTITIES_REF, BucketElementWithRef, BucketElementWithoutRef>;

        using Bucket = std::array<BucketElement, PAGE_SIZE>;
        using BucketRef = std::unique_ptr<Bucket>;
        using BucketContainer = std::vector<BucketRef>;
        using BucketIndexInfo = Index::IndexInfo;
        using AliveEntitiesContainerType = std::vector<typename EntityId::Type>;
        using AliveEntitiesContainer = std::conditional_t<HAS_ENTITIES_REF, AliveEntitiesContainerType, int>;

    public:
        CCS_EntityToComponent_Inline()
            : bucket_container_()
            , alive_entities_container_()
        {
            if constexpr (HAS_ENTITIES_REF)
                alive_entities_container_.reserve(PAGE_SIZE);
        }

    private:
        BucketContainer bucket_container_;
        AliveEntitiesContainer alive_entities_container_;

    private:
        inline BucketIndexInfo get_entity_bucketinfo(EntityId entity) const
        {
            BucketIndexInfo bucketIndexInfo;
            bucketIndexInfo.index_of_page = entity.id_ / PAGE_SIZE;
            bucketIndexInfo.component_pageindex = entity.id_ % PAGE_SIZE;
            return bucketIndexInfo;
        }

    public:
        const AliveEntitiesContainer& get_alive_container() const
        {
            return alive_entities_container_;
        }

    public:
        inline bool has_entity(EntityId entity) const
        {
            BucketIndexInfo bucketinfo = get_entity_bucketinfo(entity);

            if (bucketinfo.index_of_page >= bucket_container_.size())
                return false;

            const BucketRef& bucketref = bucket_container_[bucketinfo.index_of_page];

            if (bucketref == nullptr)
                return false;

            Index::IndexInfo indexinfo = (*bucketref)[bucketinfo.component_pageindex].indexinfo;

            return indexinfo.is_valid();
        }

        inline Index::IndexInfo get_entity_indexinfo(EntityId entity) const
        {
            BucketIndexInfo bucketinfo = get_entity_bucketinfo(entity);
            
            LECS_ASSERT(bucketinfo.index_of_page < bucket_container_.size(), "This container can have this entity")

            const BucketRef& bucketref = bucket_container_[bucketinfo.index_of_page];

            LECS_ASSERT(bucketref != nullptr, "This container can have this entity")

            return (*bucketref)[bucketinfo.component_pageindex].indexinfo;
        }

        inline void add_entity_indexinfo(EntityId entity, Index::IndexInfo indexinfo)
        {
            BucketIndexInfo bucketinfo = get_entity_bucketinfo(entity);

            if (bucketinfo.index_of_page >= bucket_container_.size())
            {
                bucket_container_.resize(bucketinfo.index_of_page + 1);
            }

            BucketRef& bucketref = bucket_container_[bucketinfo.index_of_page];

            if (bucketref == nullptr)
            {
                bucketref = std::make_unique<Bucket>();
                Bucket& bucket = *bucketref;
                for (std::size_t i = 0; i < PAGE_SIZE; ++i)
                    bucket[i].indexinfo.set_invalid();
            }

            if constexpr (HAS_ENTITIES_REF)
            {
                alive_entities_container_.emplace_back(entity.id_);
                (*bucketref)[bucketinfo.component_pageindex] = BucketElement { .indexinfo = indexinfo, .index_in_alive_list = alive_entities_container_.size() - 1 };
            }
            else
            {
                (*bucketref)[bucketinfo.component_pageindex] = BucketElement { .indexinfo = indexinfo };
            }
        }

        inline void remove_entity_indexinfo(EntityId entity)
        {
            BucketIndexInfo bucketinfo = get_entity_bucketinfo(entity);

            LECS_ASSERT(bucketinfo.index_of_page < bucket_container_.size(), "This container can have this entity")

            BucketRef& bucketref = bucket_container_[bucketinfo.index_of_page];

            LECS_ASSERT(bucketref != nullptr, "This container can have this entity")

            if constexpr (HAS_ENTITIES_REF)
            {
              Index::index_in_alive_list index_in_alive_list = (*bucketref)[bucketinfo.component_pageindex].index_in_alive_list;
                typename EntityId::Type last_entity = alive_entities_container_.back();
                alive_entities_container_[index_in_alive_list] = last_entity;
                alive_entities_container_.pop_back();
            }
            
            (*bucketref)[bucketinfo.component_pageindex].indexinfo.set_invalid();
        }
    };
}
