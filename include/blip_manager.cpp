// blip_manager.cpp
#include "blip_manager.hpp"
#include <godot_cpp/core/class_db.hpp>

void BlipManager::_bind_methods() {
    
    ClassDB::bind_method(D_METHOD("create_color_blip", "position", "color", "size"), &BlipManager::create_color_blip);
    ClassDB::bind_method(D_METHOD("create_icon_blip", "position", "icon", "size"), &BlipManager::create_icon_blip);
    ClassDB::bind_method(D_METHOD("set_blip_icon", "blip_id", "icon"), &BlipManager::set_blip_icon);
    ClassDB::bind_method(D_METHOD("set_blip_position", "blip_id", "position"), &BlipManager::set_blip_position);
    ClassDB::bind_method(D_METHOD("set_blip_color", "blip_id", "color"), &BlipManager::set_blip_color);
    ClassDB::bind_method(D_METHOD("set_blip_size", "blip_id", "size"), &BlipManager::set_blip_size);
    ClassDB::bind_method(D_METHOD("set_blip_visible", "blip_id", "visible"), &BlipManager::set_blip_visible);
    ClassDB::bind_method(D_METHOD("remove_blip", "blip_id"), &BlipManager::remove_blip);
    ClassDB::bind_method(D_METHOD("get_visible_blips", "center", "world_width", "world_height"), &BlipManager::get_visible_blips);
    ClassDB::bind_method(D_METHOD("get_blip", "blip_id"), &BlipManager::get_blip);
    ClassDB::bind_method(D_METHOD("get_blip_count"), &BlipManager::get_blip_count);
}

BlipManager::BlipManager() {
    set_name("BlipManager");
}

void BlipManager::_ready() {
    UtilityFunctions::print("=== BLIP MANAGER READY ===");
    
}

int64_t BlipManager::get_grid_key(Vector2 pos) const {
    int grid_x = (int)(pos.x / GRID_CELL_SIZE);
    int grid_y = (int)(pos.y / GRID_CELL_SIZE);
    return ((int64_t)grid_x << 32) | (grid_y & 0xFFFFFFFF);
}

void BlipManager::update_spatial_index(int blip_id, Vector2 old_pos, Vector2 new_pos) {
    int64_t old_key = get_grid_key(old_pos);
    int64_t new_key = get_grid_key(new_pos);
    
    if (old_key != new_key) {
        if (spatial_grid_.find(old_key) != spatial_grid_.end()) {
            auto& cell = spatial_grid_[old_key];
            auto it = std::find(cell.begin(), cell.end(), blip_id);
            if (it != cell.end()) {
                cell.erase(it);
            }
        }
        
        spatial_grid_[new_key].push_back(blip_id);
    }
}

int BlipManager::create_color_blip(Vector2 p_pos, Color p_color, float p_size) {
    std::lock_guard<std::mutex> lock(blips_mutex_);
    
    int blip_id = next_blip_id_++;
    
    BlipData data;
    data.id = blip_id;
    data.position = p_pos;
    data.color = p_color;
    data.size = p_size;
    data.visible = true;
    
    blips_data_[blip_id] = data;
    spatial_grid_[get_grid_key(p_pos)].push_back(blip_id);
    
    UtilityFunctions::print("Created blip ", blip_id, " at ", p_pos);
    return blip_id;
}

int BlipManager::create_icon_blip(Vector2 p_pos, Ref<Texture2D> p_icon, float p_size) {
    std::lock_guard<std::mutex> lock(blips_mutex_);
    
    int blip_id = next_blip_id_++;
    
    BlipData data;
    data.id = blip_id;
    data.position = p_pos;
    data.color = Color(1, 1, 1, 1);  // Default white
    data.size = p_size;
    data.visible = true;
    data.icon = p_icon;
    
    blips_data_[blip_id] = data;
    spatial_grid_[get_grid_key(p_pos)].push_back(blip_id);
    
    UtilityFunctions::print("Created icon blip ", blip_id, " at ", p_pos);
    return blip_id;
}

void BlipManager::set_blip_icon(int p_blip_id, Ref<Texture2D> p_icon) {
    std::lock_guard<std::mutex> lock(blips_mutex_);
    
    auto it = blips_data_.find(p_blip_id);
    if (it != blips_data_.end()) {
        it->second.icon = p_icon;
    }
}

void BlipManager::set_blip_position(int p_blip_id, Vector2 p_pos) {
    std::lock_guard<std::mutex> lock(blips_mutex_);
    
    auto it = blips_data_.find(p_blip_id);
    if (it == blips_data_.end()) return;
    
    Vector2 old_pos = it->second.position;
    it->second.position = p_pos;
    
    update_spatial_index(p_blip_id, old_pos, p_pos);
}

void BlipManager::set_blip_color(int p_blip_id, Color p_color) {
    std::lock_guard<std::mutex> lock(blips_mutex_);
    
    auto it = blips_data_.find(p_blip_id);
    if (it != blips_data_.end()) {
        it->second.color = p_color;
    }
}

void BlipManager::set_blip_size(int p_blip_id, float p_size) {
    std::lock_guard<std::mutex> lock(blips_mutex_);
    
    auto it = blips_data_.find(p_blip_id);
    if (it != blips_data_.end()) {
        it->second.size = p_size;
    }
}

void BlipManager::set_blip_visible(int p_blip_id, bool p_visible) {
    std::lock_guard<std::mutex> lock(blips_mutex_);
    
    auto it = blips_data_.find(p_blip_id);
    if (it != blips_data_.end()) {
        it->second.visible = p_visible;
    }
}

void BlipManager::remove_blip(int p_blip_id) {
    std::lock_guard<std::mutex> lock(blips_mutex_);
    
    auto it = blips_data_.find(p_blip_id);
    if (it == blips_data_.end()) return;
    
    Vector2 pos = it->second.position;
    blips_data_.erase(it);
    
    int64_t grid_key = get_grid_key(pos);
    if (spatial_grid_.find(grid_key) != spatial_grid_.end()) {
        auto& cell = spatial_grid_[grid_key];
        auto cell_it = std::find(cell.begin(), cell.end(), p_blip_id);
        if (cell_it != cell.end()) {
            cell.erase(cell_it);
        }
    }
}

Array BlipManager::get_visible_blips(Vector2 center, float world_width, float world_height) {
    std::lock_guard<std::mutex> lock(blips_mutex_);
    
    Array result;
    
    float left = center.x - world_width / 2.0f;
    float right = center.x + world_width / 2.0f;
    float top = center.y - world_height / 2.0f;
    float bottom = center.y + world_height / 2.0f;
    
    int grid_left = (int)(left / GRID_CELL_SIZE);
    int grid_right = (int)(right / GRID_CELL_SIZE);
    int grid_top = (int)(top / GRID_CELL_SIZE);
    int grid_bottom = (int)(bottom / GRID_CELL_SIZE);
    
    for (int gx = grid_left; gx <= grid_right; gx++) {
        for (int gy = grid_top; gy <= grid_bottom; gy++) {
            int64_t key = ((int64_t)gx << 32) | (gy & 0xFFFFFFFF);
            
            if (spatial_grid_.find(key) != spatial_grid_.end()) {
                for (int blip_id : spatial_grid_[key]) {
                    auto blip_it = blips_data_.find(blip_id);
                    if (blip_it != blips_data_.end() && blip_it->second.visible) {
                        Dictionary blip_dict;
                        blip_dict["id"] = blip_it->second.id;
                        blip_dict["position"] = blip_it->second.position;
                        blip_dict["icon"] = blip_it->second.icon;
                        blip_dict["color"] = blip_it->second.color;
                        blip_dict["size"] = blip_it->second.size;
                        result.append(blip_dict);
                    }
                }
            }
        }
    }
    
    return result;
}

Dictionary BlipManager::get_blip(int p_blip_id) {
    std::lock_guard<std::mutex> lock(blips_mutex_);
    
    Dictionary result;
    auto it = blips_data_.find(p_blip_id);
    
    if (it != blips_data_.end()) {
        result["id"] = it->second.id;
        result["position"] = it->second.position;
        result["icon"] = it->second.icon;
        result["color"] = it->second.color;
        result["size"] = it->second.size;
        result["visible"] = it->second.visible;
    }
    
    return result;
}

int BlipManager::get_blip_count(){

    std::lock_guard<std::mutex> lock(blips_mutex_);
    return blips_data_.size();
}
