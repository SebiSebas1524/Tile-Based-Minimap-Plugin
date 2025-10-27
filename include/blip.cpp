#include "blip.hpp"

void Blip::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_color", "color"), &Blip::set_color);
    ClassDB::bind_method(D_METHOD("get_color"), &Blip::get_color);
    ADD_PROPERTY(PropertyInfo(Variant::COLOR, "color"), "set_color", "get_color");

    ClassDB::bind_method(D_METHOD("set_size", "size"), &Blip::set_size);
    ClassDB::bind_method(D_METHOD("get_size"), &Blip::get_size);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "size"), "set_size", "get_size");

    ClassDB::bind_method(D_METHOD("set_blip_id", "id"), &Blip::set_blip_id);
    ClassDB::bind_method(D_METHOD("get_blip_id"), &Blip::get_blip_id);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "blip_id"), "set_blip_id", "get_blip_id");
}


void Blip::set_color(Color p_color) { color = p_color; }
Color Blip::get_color() const { return color; }

void Blip::set_size(float p_size) { size = p_size; }
float Blip::get_size() const { return size; }

void Blip::set_blip_id(int p_id) { blip_id = p_id; }
int Blip::get_blip_id() const { return blip_id; }