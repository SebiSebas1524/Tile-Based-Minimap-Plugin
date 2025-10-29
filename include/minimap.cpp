#include "minimap.hpp"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/window.hpp>

using namespace godot;

void Minimap::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_tile_size", "size"), &Minimap::set_tile_size);
    ClassDB::bind_method(D_METHOD("get_tile_size"), &Minimap::get_tile_size);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "tile_size"), "set_tile_size", "get_tile_size");

    ClassDB::bind_method(D_METHOD("set_tile_amount_x", "amount"), &Minimap::set_tile_amount_x);
    ClassDB::bind_method(D_METHOD("get_tile_amount_x"), &Minimap::get_tile_amount_x);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "tile_amount_x", PROPERTY_HINT_RANGE, "1,20,1"), "set_tile_amount_x", "get_tile_amount_x");

    ClassDB::bind_method(D_METHOD("set_tile_amount_y", "amount"), &Minimap::set_tile_amount_y);
    ClassDB::bind_method(D_METHOD("get_tile_amount_y"), &Minimap::get_tile_amount_y);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "tile_amount_y", PROPERTY_HINT_RANGE, "1,20,1"), "set_tile_amount_y", "get_tile_amount_y");

    ClassDB::bind_method(D_METHOD("set_init_position", "pos"), &Minimap::set_init_position);
    ClassDB::bind_method(D_METHOD("get_init_position"), &Minimap::get_init_position);
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "init_position"), "set_init_position", "get_init_position");

    ClassDB::bind_method(D_METHOD("set_minimap_zoom", "zoom"), &Minimap::set_minimap_zoom);
    ClassDB::bind_method(D_METHOD("get_minimap_zoom"), &Minimap::get_minimap_zoom);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "minimap_zoom" , PROPERTY_HINT_RANGE, "1,50,0.2"), "set_minimap_zoom", "get_minimap_zoom");

    ClassDB::bind_method(D_METHOD("get_folder_path"), &Minimap::get_folder_path);
    ClassDB::bind_method(D_METHOD("set_folder_path", "path"), &Minimap::set_folder_path);
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "folder_path", PROPERTY_HINT_DIR), "set_folder_path", "get_folder_path");

    ClassDB::bind_method(D_METHOD("set_load_map_key", "key"), &Minimap::set_load_map_key);
    ClassDB::bind_method(D_METHOD("get_load_map_key"), &Minimap::get_load_map_key);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "load_map_key"), "set_load_map_key", "get_load_map_key");

    ClassDB::bind_method(D_METHOD("reload_full_map"), &Minimap::reload_full_map);
    ClassDB::bind_method(D_METHOD("enter_full_map_view"), &Minimap::enter_full_map_view);
    ClassDB::bind_method(D_METHOD("exit_full_map_view"), &Minimap::exit_full_map_view);
    ClassDB::bind_method(D_METHOD("get_is_full_map_view"), &Minimap::get_is_full_map_view);

    ClassDB::bind_method(D_METHOD("_thread_load_tile", "x", "y", "path"), &Minimap::_thread_load_tile);
    ClassDB::bind_method(D_METHOD("on_tile_loaded", "x", "y", "texture"), &Minimap::on_tile_loaded);
    
    ClassDB::bind_method(D_METHOD("set_blip_manager", "manager"), &Minimap::set_blip_manager);
    ClassDB::bind_method(D_METHOD("get_blip_manager"), &Minimap::get_blip_manager);
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "blip_manager", PROPERTY_HINT_NODE_TYPE, "BlipManager"), 
                 "set_blip_manager", "get_blip_manager");
}

Minimap::Minimap() : folder_path("") {
    this->set_clip_contents(true);
}

Minimap::~Minimap() {
    tiles_being_loaded_.clear();
    tiles_textures_.clear();
}

void Minimap::_notification(int p_what) {
    switch (p_what) {
        case NOTIFICATION_READY:
            _ready();
            break;
        case NOTIFICATION_PROCESS:
            _process(get_process_delta_time());
            break;
    }
}

void Minimap::_ready() {
    UtilityFunctions::print("=== MINIMAP READY ===");
    set_process(true);
    
    if (!blip_manager) {
       blip_manager = get_node<BlipManager>("../BlipManager");
    }
    if (!blip_manager) {
        UtilityFunctions::push_warning("BlipManager not found!");
    }
    
    Camera3D *cam = get_viewport()->get_camera_3d();
    if (!cam) {
        UtilityFunctions::push_warning("Camera not found yet");
        return;
    }
    
    if (cam->is_inside_tree()) {
        last_cam_pos = cam->get_global_position();
        update_visible_tiles(cam);
        queue_redraw();
    }
}

void Minimap::_process(double delta) {
 if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    Input *input = Input::get_singleton();
    
    bool is_key_pressed_now = input->is_key_pressed(load_map_key);

    if (is_key_pressed_now && !was_pressed) {
        if (!is_full_map_view) {
            enter_full_map_view();
        } else {
            exit_full_map_view();
        }
        was_pressed = true;
    } else if (!is_key_pressed_now && was_pressed) {
        was_pressed = false;
    }

    if (is_full_map_view) {
        return; 
    }
    
    Camera3D *cam = get_viewport()->get_camera_3d();
    if (!cam) return;

    Vector3 current_pos = cam->get_global_position();
    
    // Track both position AND rotation changes
    float current_yaw = cam->get_global_transform().basis.get_euler().y;
    bool position_changed = (current_pos != last_cam_pos);
    bool rotation_changed = (current_yaw != last_cam_yaw);
    
    if (position_changed) {
        last_cam_pos = current_pos;
        update_visible_tiles(cam);
    }
    
    if (position_changed || rotation_changed) {
        last_cam_yaw = current_yaw;
        queue_redraw();
    }
}

void Minimap::update_visible_tiles(Camera3D *cam) {
    
    Vector3 cam_pos = cam->get_global_position();


    Vector2 minimap_center = get_size() / 2.0;
    Rect2 minimap_rect(Vector2(0, 0), get_size());
    
    // Calculate world bounds that COULD be visible
    float world_half_width = (get_size().x / 2.0) / minimap_zoom;
    float world_half_height = (get_size().y / 2.0) / minimap_zoom;
    
    float world_left = cam_pos.x - world_half_width;
    float world_right = cam_pos.x + world_half_width;
    float world_top = cam_pos.z - world_half_height;
    float world_bottom = cam_pos.z + world_half_height;
    
    // Convert to tile indices
    int tile_x_min = std::max(0, (int)floor((world_left - init_position.x) / tile_world_size) );
    int tile_x_max = std::min(tile_amount_x - 1, (int)ceil((world_right - init_position.x) / tile_world_size) );
    int tile_y_min = std::max(0, (int)floor((world_top - init_position.z) / tile_world_size) );
    int tile_y_max = std::min(tile_amount_y - 1, (int)ceil((world_bottom - init_position.z) / tile_world_size) );
    
    std::set<std::pair<int, int>> tiles_in_view;
    
    // Only check tiles in the calculated range
    for (int x = tile_x_min; x <= tile_x_max; x++) {
        for (int y = tile_y_min; y <= tile_y_max; y++) {
            tiles_in_view.insert({x, y});

            std::lock_guard<std::mutex> lock(tiles_mutex_);
            std::lock_guard<std::mutex> loading_lock(loading_mutex_);
       
            bool is_loaded = tiles_textures_.find({x, y}) != tiles_textures_.end();
            bool is_loading = tiles_being_loaded_.find({x, y}) != tiles_being_loaded_.end();
            if (!is_loaded && !is_loading) {
                load_single_tile_async(x, y);
                
                //Under here is the synchronous version
                //load_single_tile(x, y);
            }
        }
    }
    
    // Unload distant tiles
    std::vector<std::pair<int, int>> to_unload;
    {
        std::lock_guard<std::mutex> lock(tiles_mutex_);
        for (const auto& [tile_idx, tex] : tiles_textures_) {
            if (tiles_in_view.find(tile_idx) == tiles_in_view.end()) {
                to_unload.push_back(tile_idx);
                UtilityFunctions::print("Marking for unload: ", tile_idx.first, ", ", tile_idx.second);
            }
        }
    }
    
    for (const auto& idx : to_unload) {
        unload_single_tile(idx.first, idx.second);
    }
}

void Minimap::load_single_tile(int x, int y) {
    // Don't reload if already loaded
    if (tiles_textures_.find({x, y}) != tiles_textures_.end()) {
        return;
    }
    
    String path = vformat("%s/tile_%d_%d.png", folder_path, x, y);
    
    if (!ResourceLoader::get_singleton()->exists(path)) {
        return;  // File doesn't exist, skip
    }
    
    Ref<Texture2D> tex = ResourceLoader::get_singleton()->load(path, "Texture2D");
    
    if (tex.is_valid()) {
        tiles_textures_[{x, y}] = tex;
    }
}

void Minimap::unload_single_tile(int x, int y) {
    std::lock_guard<std::mutex> lock(tiles_mutex_);
    auto it = tiles_textures_.find({x, y});
    if (it != tiles_textures_.end()) {
        tiles_textures_.erase(it);
    } else {
        UtilityFunctions::print("Tried to unload non-existent tile: ", x, ", ", y);
    }
}

void Minimap::load_single_tile_async(int x, int y) {
    {
        tiles_being_loaded_.insert({x, y});
    }
    
    String path = vformat("%s/tile_%d_%d.png", folder_path, x, y);
    
    Callable task = callable_mp(this, &Minimap::_thread_load_tile);
    task = task.bind(x, y, path);
    
    WorkerThreadPool::get_singleton()->add_task(task, false, vformat("LoadTile_%d_%d", x, y));
    
    UtilityFunctions::print("Started async load for tile: ", x, ", ", y);
}

void Minimap::_thread_load_tile(int x, int y, String path) {
    // This runs on background thread
    Ref<Texture2D> tex;
    
    if (ResourceLoader::get_singleton()->exists(path)) {
        tex = ResourceLoader::get_singleton()->load(path, "Texture2D");
    }
    
    // Return to main thread
    call_deferred("on_tile_loaded", x, y, tex);
}

void Minimap::on_tile_loaded(int x, int y, Ref<Texture2D> texture) {
    {
        std::lock_guard<std::mutex> lock(loading_mutex_);
        tiles_being_loaded_.erase({x, y});
    }
    
    if (texture.is_valid()) {
        std::lock_guard<std::mutex> lock(tiles_mutex_);
        tiles_textures_[{x, y}] = texture;
        UtilityFunctions::print("Async loaded tile: ", x, ", ", y);
    }
        
    queue_redraw();
}

void Minimap::_draw() {
    Camera3D *cam = get_viewport()->get_camera_3d();
    if (!cam) return;
    
    Vector3 cam_pos = cam->get_global_position();
    Vector2 minimap_center = get_size() / 2.0;
    
    std::lock_guard<std::mutex> lock(tiles_mutex_);

    // Calculate the offset for the full map view
    Vector2 map_offset;
    if (is_full_map_view) {
        float total_world_width = tile_amount_x * tile_world_size;
        float total_world_height = tile_amount_y * tile_world_size;
        
        float display_width = total_world_width * minimap_zoom;
        float display_height = total_world_height * minimap_zoom;
        
        map_offset.x = (get_size().x - display_width) / 2.0;
        map_offset.y = (get_size().y - display_height) / 2.0;
    } else {
        map_offset = minimap_center;
    }

    float camera_yaw = is_full_map_view ? 0.0f : cam->get_global_transform().basis.get_euler().y;
    float cos_yaw = cos(camera_yaw);
    float sin_yaw = sin(camera_yaw);

    for (const auto& [index, tex] : tiles_textures_) {
        if (!tex.is_valid()) continue;
        
        int tile_x = index.first;   
        int tile_y = index.second;
        
        float tile_world_x = init_position.x + (tile_x * tile_world_size);
        float tile_world_z = init_position.z + (tile_y * tile_world_size);
        
        float offset_x, offset_z;
        if (is_full_map_view) {
            offset_x = (tile_world_x - init_position.x) * minimap_zoom;
            offset_z = (tile_world_z - init_position.z) * minimap_zoom;
        } else {
            offset_x = (tile_world_x - cam_pos.x) * minimap_zoom;
            offset_z = (tile_world_z - cam_pos.z) * minimap_zoom;
        }

        float rotated_x = offset_x * cos_yaw - offset_z * sin_yaw;
        float rotated_z = offset_x * sin_yaw + offset_z * cos_yaw;

        float display_size = tile_world_size * minimap_zoom;
        
        Vector2 tile_center;
        tile_center.x = map_offset.x + rotated_x;
        tile_center.y = map_offset.y + rotated_z;
        
        draw_set_transform(tile_center, camera_yaw, Vector2(1, 1));
        
        Rect2 dest_rect(Vector2(-display_size / 2.0, -display_size / 2.0), Vector2(display_size, display_size));
        draw_texture_rect(tex, dest_rect, false);
        
        draw_set_transform(Vector2(0, 0), 0, Vector2(1, 1));
    }
    
    
    // Draw blips using BlipManager
    if (blip_manager) {
        // Calculate visible world bounds
        float world_half_width = (get_size().x / 2.0) / minimap_zoom;
        float world_half_height = (get_size().y / 2.0) / minimap_zoom;
        
        Vector2 cam_pos_2d = Vector2(cam_pos.x, cam_pos.z);
        
        Array visible_blips = blip_manager->get_visible_blips(
            cam_pos_2d,
            world_half_width * 2.0,
            world_half_height * 2.0
        );
        
        UtilityFunctions::print("Visible blips: ", visible_blips.size());
        
        for (int i = 0; i < visible_blips.size(); i++) {
            Dictionary blip_data = visible_blips[i];
            Vector2 blip_pos = blip_data["position"];
            Color blip_color = blip_data["color"];
            float blip_size = blip_data["size"];
            Ref<Texture2D> icon = blip_data["icon"];
            
            float offset_x, offset_z;
            if (is_full_map_view) {
                offset_x = (blip_pos.x - init_position.x) * minimap_zoom;
                offset_z = (blip_pos.y - init_position.z) * minimap_zoom;
            } else {
                offset_x = (blip_pos.x - cam_pos.x) * minimap_zoom;
                offset_z = (blip_pos.y - cam_pos.z) * minimap_zoom;
            }
            
            float rotated_blip_x = offset_x * cos_yaw - offset_z * sin_yaw;
            float rotated_blip_z = offset_x * sin_yaw + offset_z * cos_yaw;
            
            Vector2 screen_pos = map_offset + Vector2(rotated_blip_x, rotated_blip_z);
            
            // Bounds check
            Vector2 minimap_size = get_size();
            if (screen_pos.x < 0 || screen_pos.x > minimap_size.x ||
                screen_pos.y < 0 || screen_pos.y > minimap_size.y) {
                    continue; 
                }
                
                if (icon.is_valid()) {
                    float icon_size = blip_size * minimap_zoom * 2.0;
                    Rect2 icon_rect(screen_pos - Vector2(icon_size / 2.0, icon_size / 2.0), Vector2(icon_size, icon_size));
                    draw_texture_rect(icon, icon_rect, false);
                } else {
                    draw_circle(screen_pos, blip_size * minimap_zoom, blip_color);
                }
            }
        }
        
        // Draw player position
        Vector2 player_screen_pos;
        if (is_full_map_view) {
            float player_offset_x = (cam_pos.x - init_position.x) * minimap_zoom;
            float player_offset_z = (cam_pos.z - init_position.z) * minimap_zoom;
            
            float rotated_player_x = player_offset_x * cos_yaw - player_offset_z * sin_yaw;
            float rotated_player_z = player_offset_x * sin_yaw + player_offset_z * cos_yaw;
            
            float total_world_width = tile_amount_x * tile_world_size;
            float total_world_height = tile_amount_y * tile_world_size;
            float display_width = total_world_width * minimap_zoom;
            float display_height = total_world_height * minimap_zoom;
            
            player_screen_pos.x = (get_size().x - display_width) / 2.0 + rotated_player_x;
            player_screen_pos.y = (get_size().y - display_height) / 2.0 + rotated_player_z;
        } else {
            player_screen_pos = minimap_center;
        }
        
        draw_circle(player_screen_pos, 5.0, Color(0, 1, 0));
        
        draw_rect(Rect2(Vector2(0, 0), get_size()), Color(1, 1, 1, 0.5), false, 2.0);
    }
    
    void Minimap::reload_full_map() {
        {
            std::lock_guard<std::mutex> lock(tiles_mutex_);
            tiles_textures_.clear();
        }
        
        {
            std::lock_guard<std::mutex> lock(loading_mutex_);
            tiles_being_loaded_.clear();
    }
    
    for (int x = 0; x < tile_amount_x; x++) {
        for (int y = 0; y < tile_amount_y; y++) {
            load_single_tile_async(x, y);
        }
    }
    
    queue_redraw();
    
    UtilityFunctions::print("=== RELOADING FULL MAP ===");
}

void Minimap::enter_full_map_view() {
    if (is_full_map_view) return;

    is_full_map_view = true;



    // Save current state
    saved_zoom = minimap_zoom;
    saved_offsets = get_rect();
    saved_anchor_left = get_anchor(SIDE_LEFT);
    saved_anchor_top = get_anchor(SIDE_TOP);
    saved_anchor_right = get_anchor(SIDE_RIGHT);
    saved_anchor_bottom = get_anchor(SIDE_BOTTOM);
    
    // Expand minimap to full screen
    this->set_anchors_and_offsets_preset(Control::PRESET_FULL_RECT);
    
    // Calculate zoom to fit entire map
    float total_world_width = tile_amount_x * tile_world_size;
    float total_world_height = tile_amount_y * tile_world_size;
    
    float minimap_width = get_size().x;
    float minimap_height = get_size().y;
    
    float zoom_x = minimap_width / total_world_width;
    float zoom_y = minimap_height / total_world_height;
    
    float calculated_zoom = std::min(zoom_x, zoom_y) * 0.95f;
    set_minimap_zoom(calculated_zoom);
    
    reload_full_map();

    Input::get_singleton()->set_mouse_mode(Input::MOUSE_MODE_VISIBLE);

    // Pause the scene tree (freezes all _process and _physics_process)
    get_tree()->set_pause(true);
    
    // But keep THIS node processing so the minimap still works
    set_process_mode(PROCESS_MODE_ALWAYS);
    
    UtilityFunctions::print("=== ENTERED FULL MAP VIEW ===");
}

void Minimap::exit_full_map_view() {
    if (!is_full_map_view) return;
    
    is_full_map_view = false;
    
    // Restore saved anchors
    set_anchor(SIDE_LEFT, saved_anchor_left);
    set_anchor(SIDE_TOP, saved_anchor_top);
    set_anchor(SIDE_RIGHT, saved_anchor_right);
    set_anchor(SIDE_BOTTOM, saved_anchor_bottom);
    
    // Restore saved size and position
    set_size(saved_offsets.size);
    set_position(saved_offsets.position);
    
    // Restore saved zoom
    set_minimap_zoom(saved_zoom);
    
    // Restore normal processing
    get_tree()->set_pause(false);
    set_process_mode(PROCESS_MODE_INHERIT);
    

    Input::get_singleton()->set_mouse_mode(Input::MOUSE_MODE_CAPTURED);
    
    UtilityFunctions::print("=== EXITED FULL MAP VIEW ===");
}

bool Minimap::get_is_full_map_view() const {
    return is_full_map_view;
}

void Minimap::set_tile_amount_x(int amount) {
    tile_amount_x = amount;
    queue_redraw();
}   
int Minimap::get_tile_amount_x() const {
    return tile_amount_x;
}
void Minimap::set_tile_amount_y(int amount) {
    tile_amount_y = amount;
    queue_redraw();
}
int Minimap::get_tile_amount_y() const {
    return tile_amount_y;
}
void Minimap::set_tile_size(float size) {
    tile_world_size = size;
    queue_redraw();
}
float Minimap::get_tile_size() const {
    return tile_world_size;
}
void Minimap::set_init_position(const Vector3& pos) {
    init_position = pos;
}
Vector3 Minimap::get_init_position() const {
    return init_position;
}
void Minimap::set_minimap_zoom(float zoom) {
    minimap_zoom = zoom;
    queue_redraw();
}
float Minimap::get_minimap_zoom() const {
    return minimap_zoom;
}
void Minimap::set_folder_path(const String &p_path) {
    folder_path = p_path;
    queue_redraw();
}
String Minimap::get_folder_path() const {
    return folder_path;
}
void Minimap::set_load_map_key(godot::Key p_key) {
    load_map_key = p_key;
}
godot::Key Minimap::get_load_map_key() const {
    return load_map_key;
}
void Minimap::set_blip_manager(BlipManager* p_manager) {
    blip_manager = p_manager;
}
BlipManager* Minimap::get_blip_manager() const {
    return blip_manager;
}
