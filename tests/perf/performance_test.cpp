#include "../base_lecs_tests.h"

#include "lecs/lecs.h"

#include "StreamFormat/ProfilerManager.h"

#include <set>

PCT_TEST_GROUP(LITTLE_ECS, PERFORMANCE);

struct BasicIntComponent {
    BasicIntComponent(std::size_t value = 0) : Value(value) {}

    std::size_t Value;
};

struct BasicFloatComponent {
    BasicFloatComponent(float value = 0.0f) : Value(value) {}

    float Value;
};

template <>
struct lecs::detail::ComponentStorageInfo<BasicIntComponent> : public DefaultComponentStorageInfo<BasicIntComponent>::FastComponent {
    static constexpr bool HAS_ENTITIES_REF           = false;
    static constexpr bool SEND_ENTITIES_POOL_ON_EACH = true;
};

#define BenchmarkTest(Size, Name)                                                                                         \
    PCT_TEST_FUNC(PERFORMANCE, ADD_MANY_COMPONENT_##Name) {                                                               \
        StreamFormat::ProfilerManager::Profiler profiler("ADD_MANY_COMPONENT_" #Name);                                    \
                                                                                                                          \
        lecs::Registry registry;                                                                                          \
                                                                                                                          \
        std::vector<lecs::EntityId> entities;                                                                             \
        entities.reserve(Size);                                                                                           \
                                                                                                                          \
        {                                                                                                                 \
            StreamFormat::ProfilerManager::ScopeProfile scope(profiler, "Create Entities");                               \
                                                                                                                          \
            for (std::size_t i = 0; i < Size; ++i) {                                                                      \
                entities.emplace_back(registry.create_entityid());                                                        \
            }                                                                                                             \
        }                                                                                                                 \
                                                                                                                          \
        {                                                                                                                 \
            StreamFormat::ProfilerManager::ScopeProfile scope(profiler, "Check Entities Ids");                            \
                                                                                                                          \
            bool uid = true;                                                                                              \
                                                                                                                          \
            for (std::size_t i = 0; i < Size; ++i) {                                                                      \
                if (entities[i].id_ != i) {                                                                               \
                    uid = false;                                                                                          \
                    break;                                                                                                \
                }                                                                                                         \
            }                                                                                                             \
                                                                                                                          \
            if (uid == false) {                                                                                           \
                uid = true;                                                                                               \
                                                                                                                          \
                std::set<typename lecs::EntityId::Type> setUID;                                                           \
                                                                                                                          \
                for (std::size_t i = 0; i < Size; ++i) {                                                                  \
                    if (setUID.contains(entities[i].id_)) {                                                               \
                        uid = false;                                                                                      \
                        break;                                                                                            \
                    }                                                                                                     \
                    setUID.insert(entities[i].id_);                                                                       \
                }                                                                                                         \
                                                                                                                          \
                if (uid) {                                                                                                \
                    LECS_WARN("Index are not contigus from 0");                                                           \
                }                                                                                                         \
            }                                                                                                             \
                                                                                                                          \
            PCT_ASSERT(uid);                                                                                              \
        }                                                                                                                 \
                                                                                                                          \
        {                                                                                                                 \
            StreamFormat::ProfilerManager::ScopeProfile scope(profiler, "Add Component");                                 \
                                                                                                                          \
            for (std::size_t i = 0; i < Size; ++i) {                                                                      \
                registry.Add<BasicIntComponent>(entities[i], i);                                                          \
            }                                                                                                             \
        }                                                                                                                 \
                                                                                                                          \
        {                                                                                                                 \
            StreamFormat::ProfilerManager::ScopeProfile scope(profiler, "get Component");                                 \
                                                                                                                          \
            for (std::size_t i = 0; i < Size; ++i) {                                                                      \
                PCT_EQ(i, registry.get<BasicIntComponent>(entities[i]).Value);                                            \
            }                                                                                                             \
        }                                                                                                                 \
                                                                                                                          \
        {                                                                                                                 \
            StreamFormat::ProfilerManager::ScopeProfile scope(profiler, "has Component");                                 \
                                                                                                                          \
            for (std::size_t i = 0; i < Size; ++i) {                                                                      \
                PCT_ASSERT(registry.has<BasicIntComponent>(entities[i]));                                                 \
                PCT_ASSERT(registry.has<BasicFloatComponent>(entities[i]) == false);                                      \
            }                                                                                                             \
        }                                                                                                                 \
                                                                                                                          \
        {                                                                                                                 \
            StreamFormat::ProfilerManager::ScopeProfile scope(profiler, "ForEach Component");                             \
                                                                                                                          \
            registry.foreach_unique_component<BasicIntComponent>([](lecs::EntityId, BasicIntComponent& k) { k = 5ull; }); \
        }                                                                                                                 \
                                                                                                                          \
        StreamFormat::ProfilerManager::ProfilerFactory::ToJson(profiler);                                                 \
    }

BenchmarkTest(1'000, 1K);
BenchmarkTest(10'000, 10K);
BenchmarkTest(100'000, 1'00K);
BenchmarkTest(1'000'000, 1M);
BenchmarkTest(10'000'000, 10M);
// BenchmarkTest(100'000'000, 100M);
