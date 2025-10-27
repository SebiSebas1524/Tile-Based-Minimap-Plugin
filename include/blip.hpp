// blip.hpp
#ifndef BLIP_H
#define BLIP_H

#include <godot_cpp/classes/node3d.hpp>

using namespace godot;

class Blip : public Node3D {
    GDCLASS(Blip, Node3D);

protected:
    static void _bind_methods();

private:
    Color color = Color(1, 1, 1, 1);
    float size = 5.0f;
    int blip_id = 0;

public:
    void set_color(Color p_color);
    Color get_color() const;
    
    void set_size(float p_size);
    float get_size() const;

    void set_blip_id(int p_id);
    int get_blip_id() const;
};

#endif