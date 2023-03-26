    #include "../BaseLittleECSTest.h"

#include "LittleECS/Registry/Registry.h"

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
