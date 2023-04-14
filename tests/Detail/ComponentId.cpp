#include "../BaseLittleECSTest.h"

#include "LittleECS/Detail/ComponentId.h"
#include "LittleECS/Detail/ComponentIdGenerator.h"

PCT_TEST_GROUP(LITTLE_ECS, DETAIL);
PCT_TEST_GROUP(DETAIL, COMPONENT_ID);

PCT_TEST_FUNC(COMPONENT_ID, BASIC_GLOBAL_ID_GEN)
{
    auto intID = LECS::Detail::GlobalComponentIdGenerator::GetTypeId<int>();
    PCT_EQ(intID, LECS::Detail::GlobalComponentIdGenerator::GetTypeId<int>());
    
    auto floatID = LECS::Detail::GlobalComponentIdGenerator::GetTypeId<float>();
    PCT_EQ(intID, LECS::Detail::GlobalComponentIdGenerator::GetTypeId<int>());
    PCT_EQ(floatID, LECS::Detail::GlobalComponentIdGenerator::GetTypeId<float>());

    PCT_NEQ(intID, floatID);
}
