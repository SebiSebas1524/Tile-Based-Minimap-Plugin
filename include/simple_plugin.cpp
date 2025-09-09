#include "simple_plugin.hpp"

#include "mathf.hpp"

void tbm_plugin::_bind_methods()
{
    ClassDB::bind_static_method("tbm_plugin",
                                D_METHOD("lerp", "a", "b", "t"),
                                &tbm_plugin::lerp);

    ClassDB::bind_static_method("godot_cpp_plugin",
                                D_METHOD("inverse_lerp", "a", "b", "v"),
                                &tbm_plugin::inverse_lerp);
}

float tbm_plugin::lerp(float a, float b, float t)
{
    return Mathf::lerp(a, b, t);
}

float tbm_plugin::inverse_lerp(float a, float b, float v)
{
    return Mathf::inverse_lerp(a, b, v);
}