#include "minimap.hpp"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/classes/viewport.hpp>

using namespace godot;

void Minimap::_bind_methods() {
    ClassDB::bind_method(D_METHOD("load_tiles"), &Minimap::load_tiles);


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

}

Minimap::Minimap() : folder_path("") {
    this->set_clip_contents(true);
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
    scan_available_tiles();
    queue_redraw();
}

void Minimap::_process(double delta) {
    Camera3D *cam = get_viewport()->get_camera_3d();
    if (!cam) return;
    if(cam->get_global_position() != last_cam_pos) {
        last_cam_pos = cam->get_global_position();
        update_visible_tiles(cam);
    }
    queue_redraw();
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
    
    UtilityFunctions::print("Tile range: X[", tile_x_min, "-", tile_x_max, "] Y[", tile_y_min, "-", tile_y_max, "]");

    std::set<std::pair<int, int>> tiles_in_view;
    
    // Only check tiles in the calculated range
    for (int x = tile_x_min; x <= tile_x_max; x++) {
        for (int y = tile_y_min; y <= tile_y_max; y++) {
            tiles_in_view.insert({x, y});
            
            if (tiles_textures_.find({x, y}) == tiles_textures_.end()) {
                load_single_tile(x, y);
            }
        }
    }

    UtilityFunctions::print("Tiles should be visible: ", tiles_in_view.size());
    UtilityFunctions::print("Tiles currently loaded: ", tiles_textures_.size());
    
    // Unload distant tiles
    std::vector<std::pair<int, int>> to_unload;
    for (const auto& [tile_idx, tex] : tiles_textures_) {
        if (tiles_in_view.find(tile_idx) == tiles_in_view.end()) {
            to_unload.push_back(tile_idx);
            UtilityFunctions::print("Marking for unload: ", tile_idx.first, ", ", tile_idx.second);
        }
    }
    
    for (const auto& idx : to_unload) {
        unload_single_tile(idx.first, idx.second);
    }
}

// Load a single tile
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
        tiles_textures_[{x, y}] = tex;  // ← ONLY loaded tiles go here
        UtilityFunctions::print("Loaded tile: ", x, ", ", y);
    }
}

// Unload a single tile
void Minimap::unload_single_tile(int x, int y) {
    auto it = tiles_textures_.find({x, y});
    if (it != tiles_textures_.end()) {
          UtilityFunctions::print("Unloading tile: ", x, ", ", y);
        tiles_textures_.erase(it);  // ← This should remove it
    } else {
        UtilityFunctions::print("Tried to unload non-existent tile: ", x, ", ", y);
    }
}

// Optional: Scan what tiles exist on disk (run once at startup)
void Minimap::scan_available_tiles() {
    available_tiles_on_disk_.clear();
    
    for (int x = 0; x < tile_amount_x; x++) {
        for (int y = 0; y < tile_amount_y; y++) {
            String path = vformat("%s/tile_%d_%d.png", folder_path, x, y);
            if (ResourceLoader::get_singleton()->exists(path)) {
                available_tiles_on_disk_.insert({x, y});
            }
        }
    }
    
    UtilityFunctions::print("Found ", available_tiles_on_disk_.size(), " tiles on disk");
}

void Minimap::_draw() {
    Camera3D *cam = get_viewport()->get_camera_3d();
    if (!cam) return;
    
    Vector3 cam_pos = cam->get_global_position();
    Vector2 minimap_center = get_size() / 2.0;
    
    for (const auto& [index, tex] : tiles_textures_) {
        if (!tex.is_valid()) continue;
        
        int tile_x = index.first;   
        int tile_y = index.second;
        
        // World position this tile represents (CENTER of tile)
        float tile_world_x = init_position.x + (tile_x * tile_world_size) + (tile_world_size / 2.0);
        float tile_world_z = init_position.z + (tile_y * tile_world_size) + (tile_world_size / 2.0);
        
        // Offset from camera (in world units, then scaled to screen)
        float offset_x = (tile_world_x - cam_pos.x) * minimap_zoom;
        float offset_z = (tile_world_z - cam_pos.z) * minimap_zoom;
        
        // Tile display size (world size scaled to screen)
        float display_size = tile_world_size * minimap_zoom;
        
        // Position on screen (centered on calculated position)
        Vector2 screen_pos;
        screen_pos.x = minimap_center.x + offset_x - (display_size / 2.0);
        screen_pos.y = minimap_center.y + offset_z - (display_size / 2.0);
        
        Rect2 dest_rect(screen_pos, Vector2(display_size, display_size));
        
        draw_texture_rect(tex, dest_rect, false);
    }
    
    draw_circle(minimap_center, 5.0, Color(0, 1, 0));
    draw_rect(Rect2(Vector2(0, 0), get_size()), Color(1, 1, 1, 0.5), false, 2.0);
}

void Minimap::load_tiles() {
    UtilityFunctions::print("=== LOADING TILES ===");
    
    for (int x = 0; x < tile_amount_x; x++) {
        for (int y = 0; y < tile_amount_y; y++) {
            String path = vformat("res://tiles/tile_%d_%d.png", x, y);
            
            UtilityFunctions::print("Attempting to load: ", path);
            
            // Check if file exists first
            if (!ResourceLoader::get_singleton()->exists(path)) {
                UtilityFunctions::print("WARNING: File does not exist: ", path);
                continue;
            }
            
            Ref<Texture2D> tex = ResourceLoader::get_singleton()->load(path, "Texture2D");
            
            if (tex.is_valid()) {
                tiles_textures_[std::make_pair(x, y)] = tex;
                UtilityFunctions::print("Successfully loaded tile: ", path, " | Size: ", tex->get_size());
            } else {
                UtilityFunctions::print("ERROR: Failed to load texture: ", path);
            }
        }
    }

    UtilityFunctions::print("Total tiles loaded: ", tiles_textures_.size());
    UtilityFunctions::print("===================");
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
