#ifndef MINIMAP_HPP
#define MINIMAP_HPP

#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/classes/camera3d.hpp>
#include <map>
#include <utility>



using namespace godot;

class Minimap : public Control {

    GDCLASS(Minimap, Control)

private:
    String folder_path;

    std::map<std::pair<int, int>, Ref<Texture2D>> tiles_textures_;
    
    std::set<std::pair<int, int>> available_tiles_on_disk_;
    int tile_amount_x = 3;  
    int tile_amount_y = 3;
    float tile_world_size = 20.0; 
    float minimap_zoom = 6.0;  
    Vector3 init_position = Vector3(0, 0, 0);
    Vector3 last_cam_pos = Vector3(999999, 999999, 999999);
protected:
    static void _bind_methods();

public:
    Minimap();
    
    void _notification(int p_what); 
    void _ready();
    void _process(double delta);
    void update_visible_tiles(Camera3D *cam);
    void _draw();
    void scan_available_tiles();
    void load_single_tile(int x, int y);
    void unload_single_tile(int x, int y);
    void load_tiles();

    int get_tile_amount_x() const;
    void set_tile_amount_x(int amount);
    int get_tile_amount_y() const;
    void set_tile_amount_y(int amount);
    float get_tile_size() const;
    void set_tile_size(float size);
    Vector3 get_init_position() const;
    void set_init_position(const Vector3& pos); 
    float get_minimap_zoom() const;
    void set_minimap_zoom(float zoom);

    void set_folder_path(const String &p_path);
    String get_folder_path() const;

};

#endif