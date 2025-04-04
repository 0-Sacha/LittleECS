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

SFT_TEST_GROUP(LITTLE_ECS, BASIC_WORKFLOW);

struct BasicFloatComponent {
    explicit BasicFloatComponent(float value_arg = 0.0f) : value(value_arg) {}

    float value;
};

struct BasicIntComponentFC {
    explicit BasicIntComponentFC(std::size_t value_arg = 0) : value(value_arg) {}

    std::size_t value;
};
template <>
struct lecs::detail::ComponentStorageInfo<BasicIntComponentFC> : public DefaultComponentStorageInfo<BasicIntComponentFC>::FastComponent {};

struct BasicIntComponentFCNREF {
    explicit BasicIntComponentFCNREF(std::size_t value_arg = 0) : value(value_arg) {}

    std::size_t value;
};
template <>
struct lecs::detail::ComponentStorageInfo<BasicIntComponentFCNREF> : public DefaultComponentStorageInfo<BasicIntComponentFCNREF>::FastComponentWithoutREF {};

struct BasicIntComponentCC {
    explicit BasicIntComponentCC(std::size_t value_arg = 0) : value(value_arg) {}

    std::size_t value;
};
template <>
struct lecs::detail::ComponentStorageInfo<BasicIntComponentCC> : public DefaultComponentStorageInfo<BasicIntComponentCC>::CommonComponent {};

struct BasicIntComponentRC {
    explicit BasicIntComponentRC(std::size_t value_arg = 0) : value(value_arg) {}

    std::size_t value;
};
template <>
struct lecs::detail::ComponentStorageInfo<BasicIntComponentRC> : public DefaultComponentStorageInfo<BasicIntComponentRC>::RareComponent {};

#define BasicWorkflow(Postfix_ComponentToUse)                                                                                                                                  \
    SFT_TEST_FUNC(BASIC_WORKFLOW, BASIC_WORK_FLOW_TEST##Postfix_ComponentToUse) {                                                                                              \
        lecs::Registry        registry;                                                                                                                                        \
        const lecs::Registry& constRegistry = registry;                                                                                                                        \
                                                                                                                                                                               \
        lecs::EntityId entity1 = registry.create_entityid();                                                                                                                   \
        lecs::EntityId entity2 = registry.create_entityid();                                                                                                                   \
        lecs::EntityId entity3 = registry.create_entityid();                                                                                                                   \
        SFT_NEQ(entity1.id_, entity2.id_);                                                                                                                                     \
        SFT_NEQ(entity1.id_, entity3.id_);                                                                                                                                     \
        SFT_NEQ(entity2.id_, entity3.id_);                                                                                                                                     \
                                                                                                                                                                               \
        registry.add<BasicIntComponent##Postfix_ComponentToUse>(entity1, 7ull);                                                                                                \
        registry.add<BasicIntComponent##Postfix_ComponentToUse>(entity2, 101ull);                                                                                              \
        registry.add<int>(entity2, 101);                                                                                                                                       \
        registry.add<float>(entity2, 101.0f);                                                                                                                                  \
                                                                                                                                                                               \
        SFT_EQ(registry.get<BasicIntComponent##Postfix_ComponentToUse>(entity1).value, 7ull);                                                                                  \
        SFT_EQ(registry.get<BasicIntComponent##Postfix_ComponentToUse>(entity2).value, 101ull);                                                                                \
        SFT_ASSERT(registry.has<BasicIntComponent##Postfix_ComponentToUse>(entity3) == false);                                                                                 \
                                                                                                                                                                               \
        registry.add<BasicFloatComponent>(entity1, 171.0f);                                                                                                                    \
        registry.add<BasicFloatComponent>(entity3, 5.0f);                                                                                                                      \
                                                                                                                                                                               \
        SFT_EQ(registry.get<BasicFloatComponent>(entity1).value, 171.0f);                                                                                                      \
        SFT_EQ(registry.get<BasicFloatComponent>(entity3).value, 5.0f);                                                                                                        \
        SFT_ASSERT(registry.has<BasicFloatComponent>(entity2) == false);                                                                                                       \
        SFT_EQ(registry.get<BasicIntComponent##Postfix_ComponentToUse>(entity1).value, 7ull);                                                                                  \
        SFT_EQ(registry.get<BasicIntComponent##Postfix_ComponentToUse>(entity2).value, 101ull);                                                                                \
        SFT_ASSERT(registry.has<BasicIntComponent##Postfix_ComponentToUse>(entity3) == false);                                                                                 \
                                                                                                                                                                               \
        registry.foreach_components<BasicIntComponent##Postfix_ComponentToUse, BasicFloatComponent>([](BasicIntComponent##Postfix_ComponentToUse& k, BasicFloatComponent& v) { \
            k = BasicIntComponent##Postfix_ComponentToUse{325ull};                                                                                                             \
            v = BasicFloatComponent{22.0f};                                                                                                                                    \
        });                                                                                                                                                                    \
                                                                                                                                                                               \
        constRegistry.foreach_components<BasicIntComponent##Postfix_ComponentToUse, BasicFloatComponent>(                                                                      \
            [&link](const BasicIntComponent##Postfix_ComponentToUse& k, const BasicFloatComponent& v) {                                                                        \
            SFT_EQ(k.value, 325ull);                                                                                                                                           \
            SFT_EQ(v.value, 22.0f);                                                                                                                                            \
        });                                                                                                                                                                    \
                                                                                                                                                                               \
        registry.foreach_components<BasicIntComponent##Postfix_ComponentToUse>(                                                                                                \
            [](BasicIntComponent##Postfix_ComponentToUse& k) { k = BasicIntComponent##Postfix_ComponentToUse{85ull}; });                                                       \
                                                                                                                                                                               \
        constRegistry.foreach_components<BasicIntComponent##Postfix_ComponentToUse>([&link](const BasicIntComponent##Postfix_ComponentToUse& k) { SFT_EQ(k.value, 85ull); });  \
                                                                                                                                                                               \
        SFT_EQ(registry.get<BasicIntComponent##Postfix_ComponentToUse>(entity1).value, 85ull);                                                                                 \
        SFT_EQ(registry.get<BasicIntComponent##Postfix_ComponentToUse>(entity2).value, 85ull);                                                                                 \
        SFT_ASSERT(registry.has<BasicIntComponent##Postfix_ComponentToUse>(entity3) == false);                                                                                 \
        SFT_EQ(registry.get<BasicFloatComponent>(entity1).value, 22.0f);                                                                                                       \
        SFT_EQ(registry.get<BasicFloatComponent>(entity3).value, 5.0f);                                                                                                        \
        SFT_ASSERT(registry.has<BasicFloatComponent>(entity2) == false);                                                                                                       \
                                                                                                                                                                               \
        registry.foreach_unique_component<BasicIntComponent##Postfix_ComponentToUse>(                                                                                          \
            [](lecs::EntityId entity, BasicIntComponent##Postfix_ComponentToUse& k) { k = BasicIntComponent##Postfix_ComponentToUse{static_cast<std::size_t>(entity.id_)}; }); \
                                                                                                                                                                               \
        SFT_EQ(registry.get<BasicIntComponent##Postfix_ComponentToUse>(entity1).value, entity1.id_);                                                                           \
        SFT_ASSERT(registry.has<BasicIntComponent##Postfix_ComponentToUse>(entity2) == true);                                                                                  \
        SFT_ASSERT(registry.has<BasicIntComponent##Postfix_ComponentToUse>(entity3) == false);                                                                                 \
                                                                                                                                                                               \
        int entityCount = 0;                                                                                                                                                   \
        for (lecs::EntityId entity : registry.each_entities_with<BasicIntComponent##Postfix_ComponentToUse>()) {                                                               \
            ++entityCount;                                                                                                                                                     \
            SFT_ASSERT(registry.has<BasicIntComponent##Postfix_ComponentToUse>(entity));                                                                                       \
        }                                                                                                                                                                      \
        SFT_EQ(entityCount, 2);                                                                                                                                                \
                                                                                                                                                                               \
        auto view = registry.View<BasicIntComponent##Postfix_ComponentToUse, BasicFloatComponent>();                                                                           \
                                                                                                                                                                               \
        entityCount = 0;                                                                                                                                                       \
        for (lecs::EntityId entity : view.each_entities_with<BasicIntComponent##Postfix_ComponentToUse>()) {                                                                   \
            ++entityCount;                                                                                                                                                     \
            SFT_ASSERT(registry.has<BasicIntComponent##Postfix_ComponentToUse>(entity));                                                                                       \
        }                                                                                                                                                                      \
        SFT_EQ(entityCount, 2);                                                                                                                                                \
                                                                                                                                                                               \
        entityCount = 0;                                                                                                                                                       \
        for (auto [intComponent] : view.each_components<BasicIntComponent##Postfix_ComponentToUse>()) {                                                                        \
            ++entityCount;                                                                                                                                                     \
            intComponent = BasicIntComponent##Postfix_ComponentToUse{52ull};                                                                                                   \
        }                                                                                                                                                                      \
        SFT_EQ(entityCount, 2);                                                                                                                                                \
                                                                                                                                                                               \
        entityCount = 0;                                                                                                                                                       \
        view.foreach_components<BasicIntComponent##Postfix_ComponentToUse, BasicFloatComponent>(                                                                               \
            [&entityCount, &link, &registry](lecs::EntityId entity, BasicIntComponent##Postfix_ComponentToUse&, BasicFloatComponent&) {                                        \
            SFT_ASSERT(registry.has<BasicIntComponent##Postfix_ComponentToUse>(entity));                                                                                       \
            SFT_ASSERT(registry.has<BasicFloatComponent>(entity));                                                                                                             \
                                                                                                                                                                               \
            ++entityCount;                                                                                                                                                     \
        });                                                                                                                                                                    \
        SFT_EQ(entityCount, 1);                                                                                                                                                \
                                                                                                                                                                               \
        entityCount = 0;                                                                                                                                                       \
        view.foreach_components<BasicIntComponent##Postfix_ComponentToUse, BasicFloatComponent>(                                                                               \
            [&entityCount](BasicIntComponent##Postfix_ComponentToUse&, BasicFloatComponent&) { ++entityCount; });                                                              \
        SFT_EQ(entityCount, 1);                                                                                                                                                \
                                                                                                                                                                               \
        entityCount = 0;                                                                                                                                                       \
        for (lecs::EntityId entity : view.each_entities_with_all<BasicIntComponent##Postfix_ComponentToUse, BasicFloatComponent>()) {                                          \
            SFT_ASSERT(registry.has<BasicIntComponent##Postfix_ComponentToUse>(entity));                                                                                       \
            SFT_ASSERT(registry.has<BasicFloatComponent>(entity));                                                                                                             \
            ++entityCount;                                                                                                                                                     \
        }                                                                                                                                                                      \
        SFT_EQ(entityCount, 1);                                                                                                                                                \
                                                                                                                                                                               \
        entityCount = 0;                                                                                                                                                       \
        for (auto [intComponent, floatComponent] : view.each_components<BasicIntComponent##Postfix_ComponentToUse, BasicFloatComponent>()) {                                   \
            SFT_EQ(intComponent.value, 52ull);                                                                                                                                 \
            SFT_EQ(floatComponent.value, 22.0f);                                                                                                                               \
            ++entityCount;                                                                                                                                                     \
        }                                                                                                                                                                      \
        SFT_EQ(entityCount, 1);                                                                                                                                                \
                                                                                                                                                                               \
        SFT_EQ(registry.get<BasicIntComponent##Postfix_ComponentToUse>(entity1).value, 52ull);                                                                                 \
        SFT_EQ(registry.get<BasicIntComponent##Postfix_ComponentToUse>(entity2).value, 52ull);                                                                                 \
        SFT_ASSERT(registry.has<BasicIntComponent##Postfix_ComponentToUse>(entity3) == false);                                                                                 \
        registry.destroy_entityid(entity2);                                                                                                                                    \
        SFT_EQ(registry.get<BasicIntComponent##Postfix_ComponentToUse>(entity1).value, 52ull);                                                                                 \
        SFT_ASSERT(registry.has<BasicIntComponent##Postfix_ComponentToUse>(entity2) == false);                                                                                 \
        SFT_ASSERT(registry.has<BasicIntComponent##Postfix_ComponentToUse>(entity3) == false);                                                                                 \
    }


BasicWorkflow(FC);
BasicWorkflow(FCNREF);
BasicWorkflow(CC);
BasicWorkflow(RC);

// NOLINTEND(cppcoreguidelines-pro-bounds-constant-array-index)
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)
// NOLINTEND(modernize-avoid-c-arrays)
// NOLINTEND(hicpp-avoid-c-arrays)
// NOLINTEND(cppcoreguidelines-avoid-c-arrays)
// NOLINTEND(readability-magic-numbers)
// NOLINTEND(misc-const-correctness)
