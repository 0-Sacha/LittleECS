#include "../base_lecs_tests.hxx"

#include "lecs/detail/componentid.hxx"
#include "lecs/detail/componentid_generator.hxx"

// NOLINTBEGIN(misc-const-correctness)
PCT_TEST_GROUP(LITTLE_ECS, DETAIL);
PCT_TEST_GROUP(DETAIL, COMPONENT_ID);

PCT_TEST_FUNC(COMPONENT_ID, BASIC_GLOBAL_ID_GEN) {
    auto int_id = lecs::detail::GlobalComponentIdGenerator::get_typeid<int>();
    PCT_EQ(int_id, lecs::detail::GlobalComponentIdGenerator::get_typeid<int>());

    auto float_id = lecs::detail::GlobalComponentIdGenerator::get_typeid<float>();
    PCT_EQ(int_id, lecs::detail::GlobalComponentIdGenerator::get_typeid<int>());
    PCT_EQ(float_id, lecs::detail::GlobalComponentIdGenerator::get_typeid<float>());

    PCT_NEQ(int_id, float_id);
}
// NOLINTEND(misc-const-correctness)
