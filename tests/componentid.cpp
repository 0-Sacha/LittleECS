#include "base_lecs_tests.h"

#include "lecs/detail/componentid.h"
#include "lecs/detail/componentid_generator.h"

PCT_TEST_GROUP(LITTLE_ECS, DETAIL);
PCT_TEST_GROUP(DETAIL, COMPONENT_ID);

PCT_TEST_FUNC(COMPONENT_ID, BASIC_GLOBAL_ID_GEN)
{
    auto intID = lecs::detail::GlobalComponentIdGenerator::get_typeid<int>();
    PCT_EQ(intID, lecs::detail::GlobalComponentIdGenerator::get_typeid<int>());
    
    auto floatID = lecs::detail::GlobalComponentIdGenerator::get_typeid<float>();
    PCT_EQ(intID, lecs::detail::GlobalComponentIdGenerator::get_typeid<int>());
    PCT_EQ(floatID, lecs::detail::GlobalComponentIdGenerator::get_typeid<float>());

    PCT_NEQ(intID, floatID);
}
