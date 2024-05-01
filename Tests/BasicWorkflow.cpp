#include "BaseLittleECSTest.h"

#include "LittleECS/LittleECS.h"

#include "ProjectCore/ProfilerManager.h"

#include <set>

PCT_TEST_GROUP(LITTLE_ECS, BASIC_WORKFLOW);

struct BasicFloatComponent
{
    BasicFloatComponent(float value = 0.0f)
        : Value(value)
    {}

    float Value;
};

struct BasicIntComponentFC
{
    BasicIntComponentFC(std::size_t value = 0)
        : Value(value)
    {}

    std::size_t Value;
};
template <>
struct LECS::Detail::ComponentStorageInfo<BasicIntComponentFC> : public DefaultComponentStorageInfo<BasicIntComponentFC>::FastComponent {};

struct BasicIntComponentFCNREF
{
    BasicIntComponentFCNREF(std::size_t value = 0)
        : Value(value)
    {}

    std::size_t Value;
};
template <>
struct LECS::Detail::ComponentStorageInfo<BasicIntComponentFCNREF> : public DefaultComponentStorageInfo<BasicIntComponentFCNREF>::FastComponentWithoutREF {};

struct BasicIntComponentCC
{
    BasicIntComponentCC(std::size_t value = 0)
        : Value(value)
    {}

    std::size_t Value;
};
template <>
struct LECS::Detail::ComponentStorageInfo<BasicIntComponentCC> : public DefaultComponentStorageInfo<BasicIntComponentCC>::CommonComponent {};

struct BasicIntComponentRC
{
    BasicIntComponentRC(std::size_t value = 0)
        : Value(value)
    {}

    std::size_t Value;
};
template <>
struct LECS::Detail::ComponentStorageInfo<BasicIntComponentRC> : public DefaultComponentStorageInfo<BasicIntComponentRC>::RareComponent {};

#define BasicWorkflow(Postfix_ComponentToUse) PCT_TEST_FUNC(BASIC_WORKFLOW, BASIC_WORK_FLOW_TEST##Postfix_ComponentToUse)   \
            {                                                                                                               \
                LECS::Registry registry;                                                                                    \
                const LECS::Registry& constRegistry = registry;                                                             \
                                                                                                                            \
                LECS::EntityId entity1 = registry.CreateEntityId();                                                         \
                LECS::EntityId entity2 = registry.CreateEntityId();                                                         \
                LECS::EntityId entity3 = registry.CreateEntityId();                                                         \
                PCT_NEQ(entity1.Id, entity2.Id);                                                                            \
                PCT_NEQ(entity1.Id, entity3.Id);                                                                            \
                PCT_NEQ(entity2.Id, entity3.Id);                                                                            \
                                                                                                                            \
                registry.Add<BasicIntComponent##Postfix_ComponentToUse>(entity1, 7ull);                                     \
                registry.Add<BasicIntComponent##Postfix_ComponentToUse>(entity2, 101ull);                                   \
                registry.Add<int>(entity2, 101);                                                                            \
                registry.Add<float>(entity2, 101.0f);                                                                       \
                                                                                                                            \
                PCT_EQ(registry.Get<BasicIntComponent##Postfix_ComponentToUse>(entity1).Value, 7ull);                       \
                PCT_EQ(registry.Get<BasicIntComponent##Postfix_ComponentToUse>(entity2).Value, 101ull);                     \
                PCT_ASSERT(registry.Has<BasicIntComponent##Postfix_ComponentToUse>(entity3) == false);                      \
                                                                                                                            \
                registry.Add<BasicFloatComponent>(entity1, 171.0f);                                                         \
                registry.Add<BasicFloatComponent>(entity3, 5.0f);                                                           \
                                                                                                                            \
                PCT_EQ(registry.Get<BasicFloatComponent>(entity1).Value, 171.0f);                                           \
                PCT_EQ(registry.Get<BasicFloatComponent>(entity3).Value, 5.0f);                                             \
                PCT_ASSERT(registry.Has<BasicFloatComponent>(entity2) == false);                                            \
                PCT_EQ(registry.Get<BasicIntComponent##Postfix_ComponentToUse>(entity1).Value, 7ull);                       \
                PCT_EQ(registry.Get<BasicIntComponent##Postfix_ComponentToUse>(entity2).Value, 101ull);                     \
                PCT_ASSERT(registry.Has<BasicIntComponent##Postfix_ComponentToUse>(entity3) == false);                      \
                                                                                                                            \
                registry.ForEachComponents<BasicIntComponent##Postfix_ComponentToUse, BasicFloatComponent>(                 \
                    [](BasicIntComponent##Postfix_ComponentToUse& k, BasicFloatComponent& v)                                \
                    {                                                                                                       \
                        k = 325ull;                                                                                         \
                        v = 22.0f;                                                                                          \
                    }                                                                                                       \
                );                                                                                                          \
                                                                                                                            \
                constRegistry.ForEachComponents<BasicIntComponent##Postfix_ComponentToUse, BasicFloatComponent>(            \
                    [&link](const BasicIntComponent##Postfix_ComponentToUse& k, const BasicFloatComponent& v)               \
                    {                                                                                                       \
                        PCT_EQ(k.Value, 325ull);                                                                            \
                        PCT_EQ(v.Value, 22.0f);                                                                             \
                    }                                                                                                       \
                );                                                                                                          \
                                                                                                                            \
                registry.ForEachComponents<BasicIntComponent##Postfix_ComponentToUse>(                                      \
                    [](BasicIntComponent##Postfix_ComponentToUse& k)                                                        \
                    {                                                                                                       \
                        k = 85ull;                                                                                          \
                    }                                                                                                       \
                );                                                                                                          \
                                                                                                                            \
                constRegistry.ForEachComponents<BasicIntComponent##Postfix_ComponentToUse>(                                 \
                    [&link](const BasicIntComponent##Postfix_ComponentToUse& k)                                             \
                    {                                                                                                       \
                        PCT_EQ(k.Value, 85ull);                                                                             \
                    }                                                                                                       \
                );                                                                                                          \
                                                                                                                            \
                PCT_EQ(registry.Get<BasicIntComponent##Postfix_ComponentToUse>(entity1).Value, 85ull);                      \
                PCT_EQ(registry.Get<BasicIntComponent##Postfix_ComponentToUse>(entity2).Value, 85ull);                      \
                PCT_ASSERT(registry.Has<BasicIntComponent##Postfix_ComponentToUse>(entity3) == false);                      \
                PCT_EQ(registry.Get<BasicFloatComponent>(entity1).Value, 22.0f);                                            \
                PCT_EQ(registry.Get<BasicFloatComponent>(entity3).Value, 5.0f);                                             \
                PCT_ASSERT(registry.Has<BasicFloatComponent>(entity2) == false);                                            \
                                                                                                                            \
                registry.ForEachUniqueComponent<BasicIntComponent##Postfix_ComponentToUse>(                                 \
                    [](LECS::EntityId entity, BasicIntComponent##Postfix_ComponentToUse& k)                                 \
                    {                                                                                                       \
                        k = static_cast<std::size_t>(entity.Id);                                                            \
                    }                                                                                                       \
                );                                                                                                          \
                                                                                                                            \
                PCT_EQ(registry.Get<BasicIntComponent##Postfix_ComponentToUse>(entity1).Value, entity1.Id);                 \
                PCT_ASSERT(registry.Has<BasicIntComponent##Postfix_ComponentToUse>(entity2) == true);                       \
                PCT_ASSERT(registry.Has<BasicIntComponent##Postfix_ComponentToUse>(entity3) == false);                      \
                                                                                                                            \
                int entityCount = 0;                                                                                        \
                for (LECS::EntityId entity : registry.EachEntitiesWith<BasicIntComponent##Postfix_ComponentToUse>())        \
                {                                                                                                           \
                    ++entityCount;                                                                                          \
                    PCT_ASSERT(registry.Has<BasicIntComponent##Postfix_ComponentToUse>(entity));                            \
                }                                                                                                           \
                PCT_EQ(entityCount, 2);                                                                                     \
                                                                                                                            \
                auto view = registry.View<BasicIntComponent##Postfix_ComponentToUse, BasicFloatComponent>();                \
                                                                                                                            \
                entityCount = 0;                                                                                            \
                for (LECS::EntityId entity : view.EachEntitiesWith<BasicIntComponent##Postfix_ComponentToUse>())            \
                {                                                                                                           \
                    ++entityCount;                                                                                          \
                    PCT_ASSERT(registry.Has<BasicIntComponent##Postfix_ComponentToUse>(entity));                            \
                }                                                                                                           \
                PCT_EQ(entityCount, 2);                                                                                     \
                                                                                                                            \
                entityCount = 0;                                                                                            \
                for (auto [intComponent] : view.EachComponents<BasicIntComponent##Postfix_ComponentToUse>())                \
                {                                                                                                           \
                    ++entityCount;                                                                                          \
                    intComponent = 52ull;                                                                                   \
                }                                                                                                           \
                PCT_EQ(entityCount, 2);                                                                                     \
                                                                                                                            \
                entityCount = 0;                                                                                            \
                view.ForEachComponents<BasicIntComponent##Postfix_ComponentToUse, BasicFloatComponent>(                     \
                    [&entityCount, &link, &registry](LECS::EntityId entity, BasicIntComponent##Postfix_ComponentToUse& k, BasicFloatComponent& v)   \
                    {                                                                                                                               \
                        PCT_ASSERT(registry.Has<BasicIntComponent##Postfix_ComponentToUse>(entity));                                                \
                        PCT_ASSERT(registry.Has<BasicFloatComponent>(entity));                                                                      \
                                                                                                                                                    \
                        ++entityCount;                                                                                                              \
                    }                                                                                                                               \
                );                                                                                                                                  \
                PCT_EQ(entityCount, 1);                                                                                     \
                                                                                                                            \
                entityCount = 0;                                                                                            \
                view.ForEachComponents<BasicIntComponent##Postfix_ComponentToUse, BasicFloatComponent>(                     \
                    [&entityCount](BasicIntComponent##Postfix_ComponentToUse& k, BasicFloatComponent& v)                    \
                    {                                                                                                       \
                        ++entityCount;                                                                                      \
                    }                                                                                                       \
                );                                                                                                          \
                PCT_EQ(entityCount, 1);                                                                                     \
                                                                                                                            \
                entityCount = 0;                                                                                            \
                for (LECS::EntityId entity : view.EachEntitiesWithAll<BasicIntComponent##Postfix_ComponentToUse, BasicFloatComponent>())    \
                {                                                                                                                           \
                    PCT_ASSERT(registry.Has<BasicIntComponent##Postfix_ComponentToUse>(entity));                                            \
                    PCT_ASSERT(registry.Has<BasicFloatComponent>(entity));                                                                  \
                    ++entityCount;                                                                                                          \
                }                                                                                                                           \
                PCT_EQ(entityCount, 1);                                                                                                     \
                                                                                                                                            \
                entityCount = 0;                                                                                                            \
                for (auto [intComponent, floatComponent] : view.EachComponents<BasicIntComponent##Postfix_ComponentToUse, BasicFloatComponent>())   \
                {                                                                                                                                   \
                    PCT_EQ(intComponent.Value, 52ull);                                                                                              \
                    PCT_EQ(floatComponent.Value, 22.0f);                                                                                            \
                    ++entityCount;                                                                                                                  \
                }                                                                                                                                   \
                PCT_EQ(entityCount, 1);                                                                                                             \
                                                                                                                                                    \
                PCT_EQ(registry.Get<BasicIntComponent##Postfix_ComponentToUse>(entity1).Value, 52ull);                                              \
                PCT_EQ(registry.Get<BasicIntComponent##Postfix_ComponentToUse>(entity2).Value, 52ull);                                              \
                PCT_ASSERT(registry.Has<BasicIntComponent##Postfix_ComponentToUse>(entity3) == false);                                              \
                registry.DestroyEntityId(entity2);                                                                                                  \
                PCT_EQ(registry.Get<BasicIntComponent##Postfix_ComponentToUse>(entity1).Value, 52ull);                                              \
                PCT_ASSERT(registry.Has<BasicIntComponent##Postfix_ComponentToUse>(entity2) == false);                                              \
                PCT_ASSERT(registry.Has<BasicIntComponent##Postfix_ComponentToUse>(entity3) == false);                                              \
            }


BasicWorkflow(FC);
BasicWorkflow(FCNREF);
BasicWorkflow(CC);
BasicWorkflow(RC);
