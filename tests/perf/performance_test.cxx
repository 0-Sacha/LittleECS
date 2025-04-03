#include "../base_lecs_tests.hxx"

#include "lecs/lecs.hxx"

#include "stream/profiler.hxx"

#include <set>

// NOLINTBEGIN(misc-const-correctness)
// NOLINTBEGIN(readability-magic-numbers)
// NOLINTBEGIN(cppcoreguidelines-avoid-c-arrays)
// NOLINTBEGIN(hicpp-avoid-c-arrays)
// NOLINTBEGIN(modernize-avoid-c-arrays)
// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
// NOLINTBEGIN(cppcoreguidelines-pro-bounds-constant-array-index)

SFT_TEST_GROUP(LITTLE_ECS, PERFORMANCE);

struct BasicIntComponent {
    explicit BasicIntComponent(std::size_t value = 0) : Value(value) {}

    std::size_t Value;
};

struct BasicFloatComponent {
    explicit BasicFloatComponent(float value = 0.0f) : Value(value) {}

    float Value;
};

template <>
struct lecs::detail::ComponentStorageInfo<BasicIntComponent> : public DefaultComponentStorageInfo<BasicIntComponent>::FastComponent {
    static constexpr bool HAS_ENTITIES_REF           = false;
    static constexpr bool SEND_ENTITIES_POOL_ON_EACH = true;
};

#define BenchmarkTest(Size, Name)                                                                                                            \
    SFT_TEST_FUNC(PERFORMANCE, ADD_MANY_COMPONENT_##Name) {                                                                                  \
        StreamFormat::ProfilerManager::Profiler profiler("ADD_MANY_COMPONENT_" #Name);                                                       \
                                                                                                                                             \
        lecs::Registry registry;                                                                                                             \
                                                                                                                                             \
        std::vector<lecs::EntityId> entities;                                                                                                \
        entities.reserve(Size);                                                                                                              \
                                                                                                                                             \
        {                                                                                                                                    \
            StreamFormat::ProfilerManager::ScopeProfile scope(profiler, "Create Entities");                                                  \
                                                                                                                                             \
            for (std::size_t i = 0; i < (Size); ++i) {                                                                                       \
                entities.emplace_back(registry.create_entityid());                                                                           \
            }                                                                                                                                \
        }                                                                                                                                    \
                                                                                                                                             \
        {                                                                                                                                    \
            StreamFormat::ProfilerManager::ScopeProfile scope(profiler, "Check Entities Ids");                                               \
                                                                                                                                             \
            bool uid = true;                                                                                                                 \
                                                                                                                                             \
            for (std::size_t i = 0; i < (Size); ++i) {                                                                                       \
                if (entities[i].id_ != i) {                                                                                                  \
                    uid = false;                                                                                                             \
                    break;                                                                                                                   \
                }                                                                                                                            \
            }                                                                                                                                \
                                                                                                                                             \
            if (uid == false) {                                                                                                              \
                uid = true;                                                                                                                  \
                                                                                                                                             \
                std::set<typename lecs::EntityId::Type> setUID;                                                                              \
                                                                                                                                             \
                for (std::size_t i = 0; i < (Size); ++i) {                                                                                   \
                    if (setUID.contains(entities[i].id_)) {                                                                                  \
                        uid = false;                                                                                                         \
                        break;                                                                                                               \
                    }                                                                                                                        \
                    setUID.insert(entities[i].id_);                                                                                          \
                }                                                                                                                            \
                                                                                                                                             \
                if (uid) {                                                                                                                   \
                    LECS_WARN("Index are not contigus from 0");                                                                              \
                }                                                                                                                            \
            }                                                                                                                                \
                                                                                                                                             \
            SFT_ASSERT(uid);                                                                                                                 \
        }                                                                                                                                    \
                                                                                                                                             \
        {                                                                                                                                    \
            StreamFormat::ProfilerManager::ScopeProfile scope(profiler, "add Component");                                                    \
                                                                                                                                             \
            for (std::size_t i = 0; i < (Size); ++i) {                                                                                       \
                registry.add<BasicIntComponent>(entities[i], i);                                                                             \
            }                                                                                                                                \
        }                                                                                                                                    \
                                                                                                                                             \
        {                                                                                                                                    \
            StreamFormat::ProfilerManager::ScopeProfile scope(profiler, "get Component");                                                    \
                                                                                                                                             \
            for (std::size_t i = 0; i < (Size); ++i) {                                                                                       \
                SFT_EQ(i, registry.get<BasicIntComponent>(entities[i]).Value);                                                               \
            }                                                                                                                                \
        }                                                                                                                                    \
                                                                                                                                             \
        {                                                                                                                                    \
            StreamFormat::ProfilerManager::ScopeProfile scope(profiler, "has Component");                                                    \
                                                                                                                                             \
            for (std::size_t i = 0; i < (Size); ++i) {                                                                                       \
                SFT_ASSERT(registry.has<BasicIntComponent>(entities[i]));                                                                    \
                SFT_ASSERT(registry.has<BasicFloatComponent>(entities[i]) == false);                                                         \
            }                                                                                                                                \
        }                                                                                                                                    \
                                                                                                                                             \
        {                                                                                                                                    \
            StreamFormat::ProfilerManager::ScopeProfile scope(profiler, "ForEach Component");                                                \
                                                                                                                                             \
            registry.foreach_unique_component<BasicIntComponent>([](lecs::EntityId, BasicIntComponent& k) { k = BasicIntComponent{5ull}; }); \
        }                                                                                                                                    \
                                                                                                                                             \
        StreamFormat::ProfilerManager::ProfilerFactory::ToJson(profiler);                                                                    \
    }

BenchmarkTest(1'000, 1K);
BenchmarkTest(10'000, 10K);
BenchmarkTest(100'000, 100K);
BenchmarkTest(1'000'000, 1M);
BenchmarkTest(10'000'000, 10M);
// BenchmarkTest(100'000'000, 100M);

// NOLINTEND(cppcoreguidelines-pro-bounds-constant-array-index)
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)
// NOLINTEND(modernize-avoid-c-arrays)
// NOLINTEND(hicpp-avoid-c-arrays)
// NOLINTEND(cppcoreguidelines-avoid-c-arrays)
// NOLINTEND(readability-magic-numbers)
// NOLINTEND(misc-const-correctness)
