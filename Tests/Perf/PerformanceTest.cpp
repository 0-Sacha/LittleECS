#include "../BaseLittleECSTest.h"

#include "LittleECS/LittleECS.h"

#include "ProjectCore/ProfilerManager.h"

#include <set>

PCT_TEST_GROUP(LITTLE_ECS, PERFORMANCE);

struct BasicIntComponent
{
    BasicIntComponent(std::size_t value = 0)
        : Value(value)
    {}

    std::size_t Value;
};

struct BasicFloatComponent
{
    BasicFloatComponent(float value = 0.0f)
        : Value(value)
    {}

    float Value;
};

template <>
struct LECS::Detail::ComponentStorageInfo<BasicIntComponent> : public DefaultComponentStorageInfo<BasicIntComponent>::FastComponent
{
    static constexpr bool HAS_ENTITIES_REF = false;
    static constexpr bool SEND_ENTITIES_POOL_ON_EACH = true;
};

#define BenchmarkTest(Size, Name) PCT_TEST_FUNC(PERFORMANCE, ADD_MANY_COMPONENT_ ## Name)                                       \
                            {                                                                                                   \
                                ProjectCore::ProfilerManager::Profiler profiler("ADD_MANY_COMPONENT_" #Name);                   \
                                                                                                                                \
                                LECS::Registry registry;                                                                        \
                                                                                                                                \
                                std::vector<LECS::EntityId> entities;                                                           \
                                entities.reserve(Size);                                                                         \
                                                                                                                                \
                                {                                                                                               \
                                    ProjectCore::ProfilerManager::ScopeProfile scope(profiler, "Create Entities");              \
                                                                                                                                \
                                    for (std::size_t i = 0; i < Size; ++i)                                                      \
                                    {                                                                                           \
                                        entities.emplace_back(registry.CreateEntityId());                                       \
                                    }                                                                                           \
                                }                                                                                               \
                                                                                                                                \
                                {                                                                                               \
                                    ProjectCore::ProfilerManager::ScopeProfile scope(profiler, "Check Entities Ids");           \
                                                                                                                                \
                                    bool uid = true;                                                                            \
                                                                                                                                \
                                    for (std::size_t i = 0; i < Size; ++i)                                                      \
                                    {                                                                                           \
                                        if (entities[i].Id != i)                                                                \
                                        {                                                                                       \
                                            uid = false;                                                                        \
                                            break;                                                                              \
                                        }                                                                                       \
                                    }                                                                                           \
                                                                                                                                \
                                    if (uid == false)                                                                           \
                                    {                                                                                           \
                                        uid = true;                                                                             \
                                                                                                                                \
                                        std::set<typename LECS::EntityId::Type> setUID;                                         \
                                                                                                                                \
                                        for (std::size_t i = 0; i < Size; ++i)                                                  \
                                        {                                                                                       \
                                            if (setUID.contains(entities[i].Id))                                                \
                                            {                                                                                   \
                                                uid = false;                                                                    \
                                                break;                                                                          \
                                            }                                                                                   \
                                            setUID.insert(entities[i].Id);                                                      \
                                        }                                                                                       \
                                                                                                                                \
                                        if (uid)                                                                                \
                                        {                                                                                       \
                                            LECS_WARN("Index are not contigus from 0");                                         \
                                        }                                                                                       \
                                    }                                                                                           \
                                                                                                                                \
                                    PCT_ASSERT(uid);                                                                            \
                                }                                                                                               \
                                                                                                                                \
                                {                                                                                               \
                                    ProjectCore::ProfilerManager::ScopeProfile scope(profiler, "Add Component");                \
                                                                                                                                \
                                    for (std::size_t i = 0; i < Size; ++i)                                                      \
                                    {                                                                                           \
                                        registry.Add<BasicIntComponent>(entities[i], i);                                        \
                                    }                                                                                           \
                                }                                                                                               \
                                                                                                                                \
                                {                                                                                               \
                                    ProjectCore::ProfilerManager::ScopeProfile scope(profiler, "Get Component");                \
                                                                                                                                \
                                    for (std::size_t i = 0; i < Size; ++i)                                                      \
                                    {                                                                                           \
                                        PCT_EQ(i, registry.Get<BasicIntComponent>(entities[i]).Value);                          \
                                    }                                                                                           \
                                }                                                                                               \
                                                                                                                                \
                                {                                                                                               \
                                    ProjectCore::ProfilerManager::ScopeProfile scope(profiler, "Has Component");                \
                                                                                                                                \
                                    for (std::size_t i = 0; i < Size; ++i)                                                      \
                                    {                                                                                           \
                                        PCT_ASSERT(registry.Has<BasicIntComponent>(entities[i]));                               \
                                        PCT_ASSERT(registry.Has<BasicFloatComponent>(entities[i]) == false);                    \
                                    }                                                                                           \
                                }                                                                                               \
                                                                                                                                \
                                {                                                                                               \
                                    ProjectCore::ProfilerManager::ScopeProfile scope(profiler, "ForEach Component");            \
                                                                                                                                \
                                    registry.ForEachUniqueComponent<BasicIntComponent>([](LECS::EntityId, BasicIntComponent& k) \
                                    {                                                                                           \
                                        k = 5ull;                                                                               \
                                    });                                                                                         \
                                }                                                                                               \
                                                                                                                                \
                                ProjectCore::ProfilerManager::ProfilerFactory::ToJson(profiler);                                \
                            }

BenchmarkTest(1'000, 1K);
BenchmarkTest(10'000, 10K);
BenchmarkTest(100'000, 100K);
BenchmarkTest(1'000'000, 1M);
BenchmarkTest(10'000'000, 10M);
// BenchmarkTest(100'000'000, 100M);
