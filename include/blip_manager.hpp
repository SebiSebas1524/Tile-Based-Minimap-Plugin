#ifndef BLIP_MANAGER_H
#define BLIP_MANAGER_H

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/variant/color.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <vector>
#include <unordered_map>
#include <mutex>

using namespace godot;

struct BlipData {
    int id;
    Vector2 position;
    Ref<Texture2D> icon;
    Color color;
    float size;
    bool visible;
};

class BlipManager : public Node {
    GDCLASS(BlipManager, Node);

private:
    // Main storage: ID -> BlipData
    std::unordered_map<int, BlipData> blips_data_;
    
    // Spatial index for fast visibility queries
    std::unordered_map<int64_t, std::vector<int>> spatial_grid_;
    static const int GRID_CELL_SIZE = 256;
    
    std::mutex blips_mutex_;
    int next_blip_id_ = 0;

    // Helper functions
    int64_t get_grid_key(Vector2 pos) const;
    void update_spatial_index(int blip_id, Vector2 old_pos, Vector2 new_pos);

protected:
    static void _bind_methods();

public:
    BlipManager();  
    
    void _ready() override;

    
    // Create a new blip
    int create_color_blip(Vector2 p_pos, Color p_color, float p_size);
    int create_icon_blip(Vector2 p_pos, Ref<Texture2D> p_icon, float p_size);

    void set_blip_icon(int p_blip_id, Ref<Texture2D> p_icon);

    // Update blip position
    void set_blip_position(int p_blip_id, Vector2 p_pos);
    
    // Update blip properties
    void set_blip_color(int p_blip_id, Color p_color);
    void set_blip_size(int p_blip_id, float p_size);
    void set_blip_visible(int p_blip_id, bool p_visible);
    
    // Remove blip
    void remove_blip(int p_blip_id);
    
    // Query visible blips in region
    Array get_visible_blips(Vector2 center, float world_width, float world_height);
    
    // Get specific blip
    Dictionary get_blip(int p_blip_id);
    
    // Debug
    int get_blip_count();
};

#endif