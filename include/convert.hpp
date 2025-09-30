#pragma once

#include <string>
#include <vector>

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/variant/array.hpp>

using namespace godot;

namespace Convert
{

auto get_key_values(Array values, const String &key) -> Array
{
    std::vector<std::string> internalValues;

    for (auto i = 0; i < values.size(); i += 1)
    {
        if (values[i].get_type() == Variant::DICTIONARY)
        {
            Dictionary variant = values[i];

            Array keys = variant.keys();

            if (keys.has(key))
            {
                String value = variant[key];

                internalValues.push_back(value.utf8().get_data());
            }
        }
    }

    Array godot_values;

    for (auto const &value : internalValues)
    {
        godot_values.append(value.c_str());
    }

    return godot_values;
}

} // namespace Convert