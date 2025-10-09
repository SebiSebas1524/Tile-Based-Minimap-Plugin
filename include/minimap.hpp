#ifndef MINIMAP_HPP
#define MINIMAP_HPP

#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <map>
#include <utility>



using namespace godot;

class Minimap : public Control {
    GDCLASS(Minimap, Control)

private:
    std::map<std::pair<int, int>, Ref<Texture2D>> tiles_textures_;
    int tile_amount_x = 4;  
    int tile_amount_y = 4;
    float tile_world_size = 20.0;  // How many world units each tile represents
    float minimap_zoom = 6.0;  // Zoom level (adjust this to zoom in/out)

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