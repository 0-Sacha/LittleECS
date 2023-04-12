#include "../BaseLittleECSTest.h"

#include "LittleECS/Registry/Registry.h"

#include "ProjectCore/Instrumentation/ProfilerManger/ProfilerManger.h"

#include <set>

PCT_TEST_GROUP(LITTLE_ECS, REGISTRY);

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

PCT_TEST_FUNC(REGISTRY, BASIC_WORK_FLOW_TEST)
{
    LECS::Registry registry;
    const LECS::Registry& constRegistry = registry;

    LECS::EntityId entity1 = registry.CreateEntityId();
    LECS::EntityId entity2 = registry.CreateEntityId();
    LECS::EntityId entity3 = registry.CreateEntityId();
    PCT_NEQ(entity1.Id, entity2.Id);
    PCT_NEQ(entity1.Id, entity3.Id);
    PCT_NEQ(entity2.Id, entity3.Id);

    registry.Add<BasicIntComponent>(entity1, 7ull);
    registry.Add<BasicIntComponent>(entity2, 101ull);
    registry.Add<int>(entity2, 101);
    registry.Add<float>(entity2, 101.0f);

    PCT_EQ(registry.Get<BasicIntComponent>(entity1).Value, 7ull);
    PCT_EQ(registry.Get<BasicIntComponent>(entity2).Value, 101ull);
    PCT_ASSERT(registry.Has<BasicIntComponent>(entity3) == false);

    registry.Add<BasicFloatComponent>(entity1, 171.0f);
    registry.Add<BasicFloatComponent>(entity3, 5.0f);

    PCT_EQ(registry.Get<BasicFloatComponent>(entity1).Value, 171.0f);
    PCT_EQ(registry.Get<BasicFloatComponent>(entity3).Value, 5.0f);
    PCT_ASSERT(registry.Has<BasicFloatComponent>(entity2) == false);
    PCT_EQ(registry.Get<BasicIntComponent>(entity1).Value, 7ull);
    PCT_EQ(registry.Get<BasicIntComponent>(entity2).Value, 101ull);
    PCT_ASSERT(registry.Has<BasicIntComponent>(entity3) == false);

    registry.ForEachComponents<BasicIntComponent, BasicFloatComponent>([](BasicIntComponent& k, BasicFloatComponent& v)
    {
        k = 325ull;
        v = 22.0f;
    });

    constRegistry.ForEachComponents<BasicIntComponent, BasicFloatComponent>([&link](const BasicIntComponent& k, const BasicFloatComponent& v)
    {
        PCT_EQ(k.Value, 325ull);
        PCT_EQ(v.Value, 22.0f);
    });

    registry.ForEachComponents<BasicIntComponent>([](BasicIntComponent& k)
    {
        k = 85ull;
    });

    constRegistry.ForEachComponents<BasicIntComponent>([&link](const BasicIntComponent& k)
    {
        PCT_EQ(k.Value, 85ull);
    });

    PCT_EQ(registry.Get<BasicIntComponent>(entity1).Value, 85ull);
    PCT_EQ(registry.Get<BasicIntComponent>(entity2).Value, 85ull);
    PCT_ASSERT(registry.Has<BasicIntComponent>(entity3) == false);
    PCT_EQ(registry.Get<BasicFloatComponent>(entity1).Value, 22.0f);
    PCT_EQ(registry.Get<BasicFloatComponent>(entity3).Value, 5.0f);
    PCT_ASSERT(registry.Has<BasicFloatComponent>(entity2) == false);
    
    registry.ForEachUniqueComponent<BasicIntComponent>([](LECS::EntityId entity, BasicIntComponent& k)
    {
        k = static_cast<std::size_t>(entity.Id);
    });

    PCT_EQ(registry.Get<BasicIntComponent>(entity1).Value, entity1.Id);
    PCT_ASSERT(registry.Has<BasicIntComponent>(entity2) == true);
    PCT_ASSERT(registry.Has<BasicIntComponent>(entity3) == false);

    int entityCount = 0;
	for (LECS::EntityId entity : registry.EachEntitiesWith<BasicIntComponent>())
	{
        ++entityCount;
		PCT_ASSERT(registry.Has<BasicIntComponent>(entity));
	}
    PCT_EQ(entityCount, 2);

	auto view = registry.View<BasicIntComponent, BasicFloatComponent>();

    entityCount = 0;
	for (LECS::EntityId entity : view.EachEntitiesWith<BasicIntComponent>())
	{
        ++entityCount;
		PCT_ASSERT(registry.Has<BasicIntComponent>(entity));
	}
    PCT_EQ(entityCount, 2);

    entityCount = 0;
	for (auto [intComponent] : view.EachComponents<BasicIntComponent>())
	{
		++entityCount;
		intComponent = 52ull;
	}
    PCT_EQ(entityCount, 2);

    entityCount = 0;
    view.ForEachComponents<BasicIntComponent, BasicFloatComponent>([&entityCount, &link, &registry](LECS::EntityId entity, BasicIntComponent& k, BasicFloatComponent& v)
    {
        PCT_ASSERT(registry.Has<BasicIntComponent>(entity));
		PCT_ASSERT(registry.Has<BasicFloatComponent>(entity));

        ++entityCount;
    });
    PCT_EQ(entityCount, 1);

    entityCount = 0;
    view.ForEachComponents<BasicIntComponent, BasicFloatComponent>([&entityCount](BasicIntComponent& k, BasicFloatComponent& v)
    {
        ++entityCount;
    });
    PCT_EQ(entityCount, 1);

    entityCount = 0;
	for (LECS::EntityId entity : view.EachEntitiesWithAll<BasicIntComponent, BasicFloatComponent>())
	{
		PCT_ASSERT(registry.Has<BasicIntComponent>(entity));
		PCT_ASSERT(registry.Has<BasicFloatComponent>(entity));
        ++entityCount;
	}
    PCT_EQ(entityCount, 1);

    entityCount = 0;
	for (auto [intComponent, floatComponent] : view.EachComponents<BasicIntComponent, BasicFloatComponent>())
	{
        PCT_EQ(intComponent.Value, 52ull);
        PCT_EQ(floatComponent.Value, 22.0f);
        ++entityCount;
	}
    PCT_EQ(entityCount, 1);

	PCT_EQ(registry.Get<BasicIntComponent>(entity1).Value, 52ull);
	PCT_EQ(registry.Get<BasicIntComponent>(entity2).Value, 52ull);
    PCT_ASSERT(registry.Has<BasicIntComponent>(entity3) == false);
    registry.DestroyEntityId(entity2);
    PCT_EQ(registry.Get<BasicIntComponent>(entity1).Value, 52ull);
    PCT_ASSERT(registry.Has<BasicIntComponent>(entity2) == false);
    PCT_ASSERT(registry.Has<BasicIntComponent>(entity3) == false);
}

#define BenchmarkTest(Size, Name) PCT_TEST_FUNC(REGISTRY, ADD_MANY_COMPONENT_ ## Name)                                          \
                            {                                                                                                   \
                                ProjectCore::Instrumentation::Profiler profiler("ADD_MANY_COMPONENT_" #Name);                   \
                                                                                                                                \
                                LECS::Registry registry;                                                                   \
                                                                                                                                \
                                std::vector<LECS::EntityId> entities;                                                      \
                                entities.reserve(Size);                                                                         \
                                                                                                                                \
                                for (std::size_t i = 0; i < Size; ++i)                                                          \
                                {                                                                                               \
                                    entities.emplace_back(registry.CreateEntity());                                             \
                                }                                                                                               \
                                                                                                                                \
                                {                                                                                               \
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
                                        std::set<typename LECS::EntityId::Type> setUID;                                    \
                                                                                                                                \
			                            for (std::size_t i = 0; i < Size; ++i)                                                          \
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
                                    for (std::size_t i = 0; i < Size; ++i)                                                      \
                                    {                                                                                           \
		                                registry.Add<BasicIntComponent>(entities[i], i);                       \
                                    }                                                                                           \
                                }                                                                                               \
                                                                                                                                \
                                {                                                                                               \
		                            ProjectCore::Instrumentation::ScopeProfile scope(profiler, "Get Component");                \
                                                                                                                                \
		                            for (std::size_t i = 0; i < Size; ++i)                                                      \
		                            {                                                                                           \
			                            PCT_EQ(i, registry.Get<BasicIntComponent>(entities[i]).Value);         \
		                            }                                                                                           \
                                }                                                                                               \
                                                                                                                                \
                                {                                                                                               \
		                            ProjectCore::Instrumentation::ScopeProfile scope(profiler, "Has Component");                \
                                                                                                                                \
                                    for (std::size_t i = 0; i < Size; ++i)                                                      \
                                    {                                                                                           \
                                        PCT_ASSERT(registry.Has<BasicIntComponent>(entities[i]));                \
                                        PCT_ASSERT(registry.Has<BasicFloatComponent>(entities[i]) == false);     \
                                    }                                                                                           \
                                }                                                                                               \
                                                                                                                                \
                                {                                                                                               \
		                            ProjectCore::Instrumentation::ScopeProfile scope(profiler, "ForEach Component");            \
                                                                                                                                \
                                    registry.ForEachUniqueComponent<BasicIntComponent>([](LECS::EntityId, BasicIntComponent& k) \
                                    {                                                                                           \
                                        k = 5ull;                                                                               \
                                    });                                                                                         \
                                }                                                                                               \
                                                                                                                                \
                                ProjectCore::Instrumentation::ProfilerFactory::ToJson(profiler);                                \
                            }

// BenchmarkTest(1'000, 1K);
// BenchmarkTest(10'000, 10K);
// BenchmarkTest(100'000, 100K);
// BenchmarkTest(1'000'000, 1M);
// BenchmarkTest(10'000'000, 10M);
// BenchmarkTest(100'000'000, 100M);
