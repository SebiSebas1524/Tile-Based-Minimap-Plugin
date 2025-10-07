#include "minimap.hpp"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/classes/camera3d.hpp>
#include <godot_cpp/classes/viewport.hpp>

using namespace godot;

void Minimap::_bind_methods() {
    ClassDB::bind_method(D_METHOD("load_tiles"), &Minimap::load_tiles);
}

Minimap::Minimap() {
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
    load_tiles();
    queue_redraw();
}

void Minimap::_process(double delta) {
    queue_redraw();
}

void Minimap::_draw() {
    Camera3D *cam = get_viewport()->get_camera_3d();
    if (!cam) return;
    
    Vector3 cam_pos = cam->get_global_position();
    Vector2 minimap_center = get_size() / 2.0;
    
    // Screenshot tool's initial position is the CENTER of tile [0,0]
    Vector3 init_position = Vector3(-15, 0, -20);
    
    // Draw all tiles
    for (const auto& [index, tex] : tiles_textures_) {
        if (!tex.is_valid()) continue;
        
        int tile_x = index.first;   
        int tile_y = index.second;  
        
        // Calculate tile CENTER position based on screenshot tool's coordinate system
        // Each tile center is offset by tile_world_size from init_position
        float tile_world_x = init_position.x + (tile_x * tile_world_size);
        float tile_world_z = init_position.z + (tile_y * tile_world_size);
        
        // Calculate offset from camera to tile center
        float offset_x = (tile_world_x - cam_pos.x) * minimap_zoom;
        float offset_z = (tile_world_z - cam_pos.z) * minimap_zoom;
        
        float display_size = tile_world_size * minimap_zoom;
        
        // Position tile on screen (centered on the calculated position)
        Vector2 screen_pos;
        screen_pos.x = minimap_center.x + offset_x - (display_size / 2.0);
        screen_pos.y = minimap_center.y + offset_z - (display_size / 2.0);
        
        Rect2 dest_rect(screen_pos, Vector2(display_size, display_size));
        
        // Only draw if visible
        Rect2 minimap_rect(Vector2(0, 0), get_size());
        if (minimap_rect.intersects(dest_rect)) {
            draw_texture_rect(tex, dest_rect, false);
        }
    }
    
    // Draw camera/player indicator (green dot in center)
    draw_circle(minimap_center, 5.0, Color(0, 1, 0));
    
    // Draw minimap border
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

