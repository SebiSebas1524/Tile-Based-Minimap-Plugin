#ifndef MINIMAP_HPP
#define MINIMAP_HPP

#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <unordered_map>
#include <godot_cpp/variant/vector2i.hpp>

struct Vector2iHash {
    std::size_t operator()(const godot::Vector2i& v) const {
        return std::hash<int>()(v.x) ^ (std::hash<int>()(v.y) << 1);
    }
};

using namespace godot;

class Minimap : public Control {
    GDCLASS(Minimap, Control)

private:
    std::unordered_map<Vector2i, Ref<Texture2D>, Vector2iHash> tiles_textures_;
    int tile_amount_x = 3;  
    int tile_amount_y = 3;
    int tile_size = 64;
    float tile_world_size = 50.0;  // How many world units each tile represents
    float minimap_zoom = 1.0;  // Zoom level (adjust this to zoom in/out)

protected:
    static void _bind_methods();

public:
    Minimap();
    
    void _notification(int p_what); 
    void _ready();
    void _process(double delta);
    void _draw();
    void load_tiles();
};

#endif