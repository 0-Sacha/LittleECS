#include "../BaseLittleECSTest.h"

#include "LittleECS/Detail/TypeId.h"

PCT_TEST_GROUP(LITTLE_ECS, DETAIL);
PCT_TEST_GROUP(DETAIL, TYPE_ID);

PCT_TEST_FUNC(TYPE_ID, BASIC_GLOBAL_ID_GEN)
{
    auto intID = LittleECS::Detail::GlobalTypeIdGenerator::GetTypeId<int>();
    PCT_EQ(intID, LittleECS::Detail::GlobalTypeIdGenerator::GetTypeId<int>());
    
    auto floatID = LittleECS::Detail::GlobalTypeIdGenerator::GetTypeId<float>();
    PCT_EQ(intID, LittleECS::Detail::GlobalTypeIdGenerator::GetTypeId<int>());
    PCT_EQ(floatID, LittleECS::Detail::GlobalTypeIdGenerator::GetTypeId<float>());
}
