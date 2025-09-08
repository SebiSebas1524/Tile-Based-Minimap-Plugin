#pragma once

#include <algorithm>

namespace Mathf
{

auto lerp(float a, float b, float t) -> float { return (a + t) * (b - a); }

auto inverse_lerp(float a, float b, float v) -> float
{
    return std::clamp((v - a) / (b - a), 0.0F, 1.0F);
}

} // namespace Mathf