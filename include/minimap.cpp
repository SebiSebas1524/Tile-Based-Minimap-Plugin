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
    
    float pixels_per_world_unit = minimap_zoom;
    
    // Calculate offset to center the tile grid around world origin
    float map_width = tile_amount_x * tile_world_size;
    float map_height = tile_amount_y * tile_world_size;
    Vector3 map_world_offset(-map_width / 2.0, 0, -map_height / 2.0);
    
    // Draw all tiles
    for (const auto& [index, tex] : tiles_textures_) {
        if (!tex.is_valid()) continue;
        
        // Tile world position with offset
        Vector3 tile_world_pos(
            map_world_offset.x + (index.y * tile_world_size),
            0,
            map_world_offset.z + (index.x * tile_world_size)
        );
        
        float tile_minimap_x = tile_world_pos.z * pixels_per_world_unit;
        float tile_minimap_y = tile_world_pos.x * pixels_per_world_unit;
        
        float cam_minimap_x = cam_pos.x * pixels_per_world_unit;
        float cam_minimap_y = cam_pos.z * pixels_per_world_unit;
        
        Vector2 screen_pos;
        screen_pos.x = minimap_center.x + (tile_minimap_x - cam_minimap_x);
        screen_pos.y = minimap_center.y + (tile_minimap_y - cam_minimap_y);
        
        float display_size = tile_world_size * minimap_zoom;
        
        Rect2 dest_rect(screen_pos, Vector2(display_size, display_size));
        
        Rect2 minimap_rect(Vector2(0, 0), get_size());
        if (minimap_rect.intersects(dest_rect)) {
            draw_texture_rect(tex, dest_rect, false);
        }
    }
    
    draw_circle(minimap_center, 8.0, Color(0, 1, 0));
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
                tiles_textures_[Vector2i(x, y)] = tex;
                UtilityFunctions::print("Successfully loaded tile: ", path, " | Size: ", tex->get_size());
            } else {
                UtilityFunctions::print("ERROR: Failed to load texture: ", path);
            }
        }
    }

    UtilityFunctions::print("Total tiles loaded: ", tiles_textures_.size());
    UtilityFunctions::print("===================");
}

