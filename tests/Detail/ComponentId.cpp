#include "../BaseLittleECSTest.h"

#include "LittleECS/Detail/ComponentId.h"
#include "LittleECS/Detail/ComponentIdGenerator.h"

PCT_TEST_GROUP(LITTLE_ECS, DETAIL);
PCT_TEST_GROUP(DETAIL, COMPONENT_ID);

PCT_TEST_FUNC(COMPONENT_ID, BASIC_GLOBAL_ID_GEN)
{
    auto intID = LittleECS::Detail::GlobalComponentIdGenerator::GetTypeId<int>();
    PCT_EQ(intID, LittleECS::Detail::GlobalComponentIdGenerator::GetTypeId<int>());
    
    auto floatID = LittleECS::Detail::GlobalComponentIdGenerator::GetTypeId<float>();
    PCT_EQ(intID, LittleECS::Detail::GlobalComponentIdGenerator::GetTypeId<int>());
    PCT_EQ(floatID, LittleECS::Detail::GlobalComponentIdGenerator::GetTypeId<float>());

    PCT_NEQ(intID, floatID);
}
