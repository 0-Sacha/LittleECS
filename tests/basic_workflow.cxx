#include "base_lecs_tests.hxx"

#include "lecs/lecs.hxx"

#include "StreamFormat/ProfilerManager.h"

#include <set>

// NOLINTBEGIN(misc-const-correctness)
// NOLINTBEGIN(readability-magic-numbers)
PCT_TEST_GROUP(LITTLE_ECS, BASIC_WORKFLOW);

struct BasicFloatComponent {
    BasicFloatComponent(float value = 0.0f) : Value(value) {}

    float Value;
};

struct BasicIntComponentFC {
    BasicIntComponentFC(std::size_t value = 0) : Value(value) {}

    std::size_t Value;
};
template <>
struct lecs::detail::ComponentStorageInfo<BasicIntComponentFC> : public DefaultComponentStorageInfo<BasicIntComponentFC>::FastComponent {};

struct BasicIntComponentFCNREF {
    BasicIntComponentFCNREF(std::size_t value = 0) : Value(value) {}

    std::size_t Value;
};
template <>
struct lecs::detail::ComponentStorageInfo<BasicIntComponentFCNREF> : public DefaultComponentStorageInfo<BasicIntComponentFCNREF>::FastComponentWithoutREF {};

struct BasicIntComponentCC {
    BasicIntComponentCC(std::size_t value = 0) : Value(value) {}

    std::size_t Value;
};
template <>
struct lecs::detail::ComponentStorageInfo<BasicIntComponentCC> : public DefaultComponentStorageInfo<BasicIntComponentCC>::CommonComponent {};

struct BasicIntComponentRC {
    BasicIntComponentRC(std::size_t value = 0) : Value(value) {}

    std::size_t Value;
};
template <>
struct lecs::detail::ComponentStorageInfo<BasicIntComponentRC> : public DefaultComponentStorageInfo<BasicIntComponentRC>::RareComponent {};

#define BasicWorkflow(Postfix_ComponentToUse)                                                                                                                                  \
    PCT_TEST_FUNC(BASIC_WORKFLOW, BASIC_WORK_FLOW_TEST##Postfix_ComponentToUse) {                                                                                              \
        lecs::Registry        registry;                                                                                                                                        \
        const lecs::Registry& constRegistry = registry;                                                                                                                        \
                                                                                                                                                                               \
        lecs::EntityId entity1 = registry.create_entityid();                                                                                                                   \
        lecs::EntityId entity2 = registry.create_entityid();                                                                                                                   \
        lecs::EntityId entity3 = registry.create_entityid();                                                                                                                   \
        PCT_NEQ(entity1.id_, entity2.id_);                                                                                                                                     \
        PCT_NEQ(entity1.id_, entity3.id_);                                                                                                                                     \
        PCT_NEQ(entity2.id_, entity3.id_);                                                                                                                                     \
                                                                                                                                                                               \
        registry.add<BasicIntComponent##Postfix_ComponentToUse>(entity1, 7ull);                                                                                                \
        registry.add<BasicIntComponent##Postfix_ComponentToUse>(entity2, 101ull);                                                                                              \
        registry.add<int>(entity2, 101);                                                                                                                                       \
        registry.add<float>(entity2, 101.0f);                                                                                                                                  \
                                                                                                                                                                               \
        PCT_EQ(registry.get<BasicIntComponent##Postfix_ComponentToUse>(entity1).Value, 7ull);                                                                                  \
        PCT_EQ(registry.get<BasicIntComponent##Postfix_ComponentToUse>(entity2).Value, 101ull);                                                                                \
        PCT_ASSERT(registry.has<BasicIntComponent##Postfix_ComponentToUse>(entity3) == false);                                                                                 \
                                                                                                                                                                               \
        registry.add<BasicFloatComponent>(entity1, 171.0f);                                                                                                                    \
        registry.add<BasicFloatComponent>(entity3, 5.0f);                                                                                                                      \
                                                                                                                                                                               \
        PCT_EQ(registry.get<BasicFloatComponent>(entity1).Value, 171.0f);                                                                                                      \
        PCT_EQ(registry.get<BasicFloatComponent>(entity3).Value, 5.0f);                                                                                                        \
        PCT_ASSERT(registry.has<BasicFloatComponent>(entity2) == false);                                                                                                       \
        PCT_EQ(registry.get<BasicIntComponent##Postfix_ComponentToUse>(entity1).Value, 7ull);                                                                                  \
        PCT_EQ(registry.get<BasicIntComponent##Postfix_ComponentToUse>(entity2).Value, 101ull);                                                                                \
        PCT_ASSERT(registry.has<BasicIntComponent##Postfix_ComponentToUse>(entity3) == false);                                                                                 \
                                                                                                                                                                               \
        registry.foreach_components<BasicIntComponent##Postfix_ComponentToUse, BasicFloatComponent>([](BasicIntComponent##Postfix_ComponentToUse& k, BasicFloatComponent& v) { \
            k = 325ull;                                                                                                                                                        \
            v = 22.0f;                                                                                                                                                         \
        });                                                                                                                                                                    \
                                                                                                                                                                               \
        constRegistry.foreach_components<BasicIntComponent##Postfix_ComponentToUse, BasicFloatComponent>(                                                                      \
            [&link](const BasicIntComponent##Postfix_ComponentToUse& k, const BasicFloatComponent& v) {                                                                        \
            PCT_EQ(k.Value, 325ull);                                                                                                                                           \
            PCT_EQ(v.Value, 22.0f);                                                                                                                                            \
        });                                                                                                                                                                    \
                                                                                                                                                                               \
        registry.foreach_components<BasicIntComponent##Postfix_ComponentToUse>([](BasicIntComponent##Postfix_ComponentToUse& k) { k = 85ull; });                               \
                                                                                                                                                                               \
        constRegistry.foreach_components<BasicIntComponent##Postfix_ComponentToUse>([&link](const BasicIntComponent##Postfix_ComponentToUse& k) { PCT_EQ(k.Value, 85ull); });  \
                                                                                                                                                                               \
        PCT_EQ(registry.get<BasicIntComponent##Postfix_ComponentToUse>(entity1).Value, 85ull);                                                                                 \
        PCT_EQ(registry.get<BasicIntComponent##Postfix_ComponentToUse>(entity2).Value, 85ull);                                                                                 \
        PCT_ASSERT(registry.has<BasicIntComponent##Postfix_ComponentToUse>(entity3) == false);                                                                                 \
        PCT_EQ(registry.get<BasicFloatComponent>(entity1).Value, 22.0f);                                                                                                       \
        PCT_EQ(registry.get<BasicFloatComponent>(entity3).Value, 5.0f);                                                                                                        \
        PCT_ASSERT(registry.has<BasicFloatComponent>(entity2) == false);                                                                                                       \
                                                                                                                                                                               \
        registry.foreach_unique_component<BasicIntComponent##Postfix_ComponentToUse>(                                                                                          \
            [](lecs::EntityId entity, BasicIntComponent##Postfix_ComponentToUse& k) { k = static_cast<std::size_t>(entity.id_); });                                            \
                                                                                                                                                                               \
        PCT_EQ(registry.get<BasicIntComponent##Postfix_ComponentToUse>(entity1).Value, entity1.id_);                                                                           \
        PCT_ASSERT(registry.has<BasicIntComponent##Postfix_ComponentToUse>(entity2) == true);                                                                                  \
        PCT_ASSERT(registry.has<BasicIntComponent##Postfix_ComponentToUse>(entity3) == false);                                                                                 \
                                                                                                                                                                               \
        int entityCount = 0;                                                                                                                                                   \
        for (lecs::EntityId entity : registry.each_entities_with<BasicIntComponent##Postfix_ComponentToUse>()) {                                                               \
            ++entityCount;                                                                                                                                                     \
            PCT_ASSERT(registry.has<BasicIntComponent##Postfix_ComponentToUse>(entity));                                                                                       \
        }                                                                                                                                                                      \
        PCT_EQ(entityCount, 2);                                                                                                                                                \
                                                                                                                                                                               \
        auto view = registry.View<BasicIntComponent##Postfix_ComponentToUse, BasicFloatComponent>();                                                                           \
                                                                                                                                                                               \
        entityCount = 0;                                                                                                                                                       \
        for (lecs::EntityId entity : view.each_entities_with<BasicIntComponent##Postfix_ComponentToUse>()) {                                                                   \
            ++entityCount;                                                                                                                                                     \
            PCT_ASSERT(registry.has<BasicIntComponent##Postfix_ComponentToUse>(entity));                                                                                       \
        }                                                                                                                                                                      \
        PCT_EQ(entityCount, 2);                                                                                                                                                \
                                                                                                                                                                               \
        entityCount = 0;                                                                                                                                                       \
        for (auto [intComponent] : view.each_components<BasicIntComponent##Postfix_ComponentToUse>()) {                                                                        \
            ++entityCount;                                                                                                                                                     \
            intComponent = 52ull;                                                                                                                                              \
        }                                                                                                                                                                      \
        PCT_EQ(entityCount, 2);                                                                                                                                                \
                                                                                                                                                                               \
        entityCount = 0;                                                                                                                                                       \
        view.foreach_components<BasicIntComponent##Postfix_ComponentToUse, BasicFloatComponent>(                                                                               \
            [&entityCount, &link, &registry](lecs::EntityId entity, BasicIntComponent##Postfix_ComponentToUse& k, BasicFloatComponent& v) {                                    \
            PCT_ASSERT(registry.has<BasicIntComponent##Postfix_ComponentToUse>(entity));                                                                                       \
            PCT_ASSERT(registry.has<BasicFloatComponent>(entity));                                                                                                             \
                                                                                                                                                                               \
            ++entityCount;                                                                                                                                                     \
        });                                                                                                                                                                    \
        PCT_EQ(entityCount, 1);                                                                                                                                                \
                                                                                                                                                                               \
        entityCount = 0;                                                                                                                                                       \
        view.foreach_components<BasicIntComponent##Postfix_ComponentToUse, BasicFloatComponent>(                                                                               \
            [&entityCount](BasicIntComponent##Postfix_ComponentToUse& k, BasicFloatComponent& v) { ++entityCount; });                                                          \
        PCT_EQ(entityCount, 1);                                                                                                                                                \
                                                                                                                                                                               \
        entityCount = 0;                                                                                                                                                       \
        for (lecs::EntityId entity : view.each_entities_with_all<BasicIntComponent##Postfix_ComponentToUse, BasicFloatComponent>()) {                                          \
            PCT_ASSERT(registry.has<BasicIntComponent##Postfix_ComponentToUse>(entity));                                                                                       \
            PCT_ASSERT(registry.has<BasicFloatComponent>(entity));                                                                                                             \
            ++entityCount;                                                                                                                                                     \
        }                                                                                                                                                                      \
        PCT_EQ(entityCount, 1);                                                                                                                                                \
                                                                                                                                                                               \
        entityCount = 0;                                                                                                                                                       \
        for (auto [intComponent, floatComponent] : view.each_components<BasicIntComponent##Postfix_ComponentToUse, BasicFloatComponent>()) {                                   \
            PCT_EQ(intComponent.Value, 52ull);                                                                                                                                 \
            PCT_EQ(floatComponent.Value, 22.0f);                                                                                                                               \
            ++entityCount;                                                                                                                                                     \
        }                                                                                                                                                                      \
        PCT_EQ(entityCount, 1);                                                                                                                                                \
                                                                                                                                                                               \
        PCT_EQ(registry.get<BasicIntComponent##Postfix_ComponentToUse>(entity1).Value, 52ull);                                                                                 \
        PCT_EQ(registry.get<BasicIntComponent##Postfix_ComponentToUse>(entity2).Value, 52ull);                                                                                 \
        PCT_ASSERT(registry.has<BasicIntComponent##Postfix_ComponentToUse>(entity3) == false);                                                                                 \
        registry.destroy_entityid(entity2);                                                                                                                                    \
        PCT_EQ(registry.get<BasicIntComponent##Postfix_ComponentToUse>(entity1).Value, 52ull);                                                                                 \
        PCT_ASSERT(registry.has<BasicIntComponent##Postfix_ComponentToUse>(entity2) == false);                                                                                 \
        PCT_ASSERT(registry.has<BasicIntComponent##Postfix_ComponentToUse>(entity3) == false);                                                                                 \
    }


BasicWorkflow(FC);
BasicWorkflow(FCNREF);
BasicWorkflow(CC);
BasicWorkflow(RC);
// NOLINTEND(readability-magic-numbers)
// NOLINTEND(misc-const-correctness)
