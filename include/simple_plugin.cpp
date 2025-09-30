#include "simple_plugin.hpp"

#include "mathf.hpp"
#include "convert.hpp"

void tbm_plugin::_bind_methods()
{
    ClassDB::bind_static_method("tbm_plugin",
                                D_METHOD("lerp", "a", "b", "t"),
                                &tbm_plugin::lerp);

    ClassDB::bind_static_method("tbm_plugin",
                                D_METHOD("inverse_lerp", "a", "b", "v"),
                                &tbm_plugin::inverse_lerp);

    ClassDB::bind_static_method("tbm_plugin", 
                                D_METHOD("sum", "values"),
                                &tbm_plugin::sum);

    ClassDB::bind_static_method("tbm_plugin",
                                  D_METHOD("get_key_values", "values", "key"),
                                &tbm_plugin::get_key_values);

}


float tbm_plugin::lerp(float a, float b, float t)
{
    return Mathf::lerp(a, b, t);
}

float tbm_plugin::inverse_lerp(float a, float b, float v)
{
    return Mathf::inverse_lerp(a, b, v);
}

int tbm_plugin::sum(const Array &values)
{
    return Mathf::sum(values);
}

Array tbm_plugin::get_key_values(const Array &values, const String &key)
{
    return Convert::get_key_values(values, key);
}
