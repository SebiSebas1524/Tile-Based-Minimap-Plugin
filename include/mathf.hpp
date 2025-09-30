#pragma once

#include <algorithm>
#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/variant/array.hpp>

namespace Mathf
{

auto lerp(float a, float b, float t) -> float { return (a + t) * (b - a); }

auto inverse_lerp(float a, float b, float v) -> float
{
    return std::clamp((v - a) / (b - a), 0.0F, 1.0F);
}

using namespace godot;

auto sum(Array values) -> int
{
    auto count = 0;

    for (auto i = 0; i < values.size(); i += 1)
    {
        if (values[i].get_type() == Variant::INT)
        {
            int variant = values[i];

            count += variant;
        }
    }

    return count;
}

} // namespace Mathf