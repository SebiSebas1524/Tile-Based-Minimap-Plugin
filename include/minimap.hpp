#ifndef MINIMAP_HPP
#define MINIMAP_HPP

#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/classes/camera3d.hpp>
#include <godot_cpp/classes/worker_thread_pool.hpp>
#include <blip.hpp>
#include <mutex>
#include <map>
#include <utility>



using namespace godot;

class Minimap : public Control {

    GDCLASS(Minimap, Control)

private:

    String folder_path;
    std::map<std::pair<int, int>, Ref<Texture2D>> tiles_textures_;
    godot::Key load_map_key = KEY_M;
    bool is_full_map_view = false;  
    bool was_pressed = false;
    float saved_zoom = 0.0f;
    Rect2 saved_offsets = Rect2();
    float saved_anchor_left = 0.0f;
    float saved_anchor_top = 0.0f;
    float saved_anchor_right = 0.0f;
    float saved_anchor_bottom = 0.0f;

    // Keep track of tiles currently being loaded to avoid duplicate loads
    std::set<std::pair<int, int>> tiles_being_loaded_;
    std::mutex tiles_mutex_;
    std::mutex loading_mutex_;

    TypedArray<Object> blips;
    std::mutex blips_mutex_;

    int tile_amount_x = 3;  
    int tile_amount_y = 3;
    float tile_world_size = 20.0; 
    float minimap_zoom = 6.0;  
    Vector3 init_position = Vector3(0, 0, 0);
    Vector3 last_cam_pos = Vector3(0, 0, 0);
    float last_cam_yaw = 0.0f;


protected:
    static void _bind_methods();

public:
    Minimap();
    ~Minimap();
    
    void _notification(int p_what); 
    void _ready();
    void _process(double delta);
    void update_visible_tiles(Camera3D *cam);
    void _draw();

    
    void load_single_tile(int x, int y);
    void unload_single_tile(int x, int y);
    void load_single_tile_async(int x, int y);
    void _thread_load_tile(int x, int y, String path);
    void on_tile_loaded(int x, int y, Ref<Texture2D> texture);
    

    void reload_full_map();
    void enter_full_map_view();
    void exit_full_map_view();
    bool get_is_full_map_view() const;

    // Getters and Setters
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
    void set_load_map_key(godot::Key p_key);
    godot::Key get_load_map_key() const;

    void set_blips(TypedArray<Object> p_blips);
    TypedArray<Object> get_blips() const;
    void register_blip_nodes(Node* p_parent);
};

#endif