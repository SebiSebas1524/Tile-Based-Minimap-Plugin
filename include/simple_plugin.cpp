#include "godot_cpp_plugin.hpp"

#include "mathf.hpp"

void godot_cpp_plugin::_bind_methods()
{
    ClassDB::bind_static_method("godot_cpp_plugin",
                                D_METHOD("lerp", "a", "b", "t"),
                                &godot_cpp_plugin::lerp);

    ClassDB::bind_static_method("godot_cpp_plugin",
                                D_METHOD("inverse_lerp", "a", "b", "v"),
                                &godot_cpp_plugin::inverse_lerp);
}

float godot_cpp_plugin::lerp(float a, float b, float t)
{
    return Mathf::lerp(a, b, t);
}

float godot_cpp_plugin::inverse_lerp(float a, float b, float v)
{
    return Mathf::inverse_lerp(a, b, v);
}