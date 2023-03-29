#include "../BaseLittleECSTest.h"

#include "LittleECS/Registry/Registry.h"

#include "ProjectCore/Instrumentation/ProfilerManger/ProfilerManger.h"

#include <set>

PCT_TEST_GROUP(LITTLE_ECS, REGISTRY);

struct BasicIntComponent
{
    BasicIntComponent(int value = 0)
        : Value(value)
    {}

    int Value;
};

struct BasicFloatComponent
{
    BasicFloatComponent(float value = 0.0f)
        : Value(value)
    {}

    float Value;
};

template <>
struct LittleECS::Detail::ComponentStorageInfo<BasicIntComponent>
{
	static constexpr BasicComponentStorage::GlobalIndexOfComponent PAGE_SIZE = 512;
};

PCT_TEST_FUNC(REGISTRY, ADD_COMPONENT)
{
    LittleECS::Registry registry;

    LittleECS::EntityId entity1 = registry.CreateEntity();
    LittleECS::EntityId entity2 = registry.CreateEntity();
    LittleECS::EntityId entity3 = registry.CreateEntity();
    PCT_NEQ(entity1.Id, entity2.Id);
    PCT_NEQ(entity1.Id, entity3.Id);
    PCT_NEQ(entity2.Id, entity3.Id);

    registry.AddComponentToEntity<BasicIntComponent>(entity1, 7);
    registry.AddComponentToEntity<BasicIntComponent>(entity2, 101);

    PCT_EQ(registry.GetComponentOfEntity<BasicIntComponent>(entity1).Value, 7);
    PCT_EQ(registry.GetComponentOfEntity<BasicIntComponent>(entity2).Value, 101);
    PCT_ASSERT(registry.EntityHasComponent<BasicIntComponent>(entity3) == false);

    registry.AddComponentToEntity<BasicFloatComponent>(entity1, 171.0f);
    registry.AddComponentToEntity<BasicFloatComponent>(entity3, 5.0f);

    PCT_EQ(registry.GetComponentOfEntity<BasicFloatComponent>(entity1).Value, 171.0f);
    PCT_EQ(registry.GetComponentOfEntity<BasicFloatComponent>(entity3).Value, 5.0f);
    PCT_ASSERT(registry.EntityHasComponent<BasicFloatComponent>(entity2) == false);
    PCT_EQ(registry.GetComponentOfEntity<BasicIntComponent>(entity1).Value, 7);
    PCT_EQ(registry.GetComponentOfEntity<BasicIntComponent>(entity2).Value, 101);
    PCT_ASSERT(registry.EntityHasComponent<BasicIntComponent>(entity3) == false);
}

#define BenchmarkTest(Size, Name) PCT_TEST_FUNC(REGISTRY, ADD_MANY_COMPONENT_ ## Name)                                          \
                            {                                                                                                   \
                                ProjectCore::Instrumentation::Profiler profiler("ADD_MANY_COMPONENT_" #Name);                   \
                                                                                                                                \
                                LittleECS::Registry registry;                                                                   \
                                                                                                                                \
                                std::vector<LittleECS::EntityId> entities;                                                      \
                                entities.reserve(Size);                                                                         \
                                                                                                                                \
                                for (int i = 0; i < Size; ++i)                                                                  \
                                {                                                                                               \
                                    entities.emplace_back(registry.CreateEntity());                                             \
                                }                                                                                               \
                                                                                                                                \
                                {                                                                                               \
                                    bool uid = true;                                                                            \
                                                                                                                                \
		                            for (int i = 0; i < Size; ++i)                                                              \
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
                                        std::set<typename LittleECS::EntityId::Type> setUID;                                    \
                                                                                                                                \
			                            for (int i = 0; i < Size; ++i)                                                          \
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
                                    ProjectCore::Instrumentation::ScopeProfile scope(profiler, "Add Component");                \
                                                                                                                                \
                                    for (int i = 0; i < Size; ++i)                                                              \
                                    {                                                                                           \
		                                registry.AddComponentToEntity<BasicIntComponent>(entities[i], i);                       \
                                    }                                                                                           \
                                }                                                                                               \
                                                                                                                                \
                                {                                                                                               \
		                            ProjectCore::Instrumentation::ScopeProfile scope(profiler, "Get Component");                \
                                                                                                                                \
		                            for (int i = 0; i < Size; ++i)                                                              \
		                            {                                                                                           \
			                            PCT_EQ(i, registry.GetComponentOfEntity<BasicIntComponent>(entities[i]).Value);         \
		                            }                                                                                           \
                                }                                                                                               \
                                                                                                                                \
                                {                                                                                               \
		                            ProjectCore::Instrumentation::ScopeProfile scope(profiler, "Has Component");                \
                                                                                                                                \
                                    for (int i = 0; i < Size; ++i)                                                              \
                                    {                                                                                           \
                                        PCT_ASSERT(registry.EntityHasComponent<BasicIntComponent>(entities[i]));                \
                                        PCT_ASSERT(registry.EntityHasComponent<BasicFloatComponent>(entities[i]) == false);     \
                                    }                                                                                           \
                                }                                                                                               \
                                                                                                                                \
                                ProjectCore::Instrumentation::ProfilerFactory::ToJson(profiler);                                \
                            }

BenchmarkTest(10'000, 10K);
BenchmarkTest(100'000, 100K);
BenchmarkTest(1'000'000, 1M);
// BenchmarkTest(100'000'000, 100M);
