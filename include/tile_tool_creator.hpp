#ifndef SCREENSHOT_EXTENSION_HPP
#define SCREENSHOT_EXTENSION_HPP

#include <godot_cpp/classes/editor_plugin.hpp>
#include <godot_cpp/classes/sub_viewport.hpp>

namespace godot
{

class TileToolCreator : public Node {
    GDCLASS(TileToolCreator, Node);

private:
    float tile_size = 20.0f;
    int tile_amount_x = 3;
    int tile_amount_y = 3;
    Vector3 init_position = Vector3(-15, 0, -20);

    int current_x = 0;
    int current_z = 0;
    float wait_time = 1.0f;
    float elapsed = 0.0f;
    bool capturing = false;
    bool ready_to_capture = false;

    SubViewport *sub_vp = nullptr;
    Camera3D *camera = nullptr;

protected:
    static void _bind_methods();

public:
    // Getters
    float get_tile_size() const { return tile_size; }
    int get_tile_amount_x() const { return tile_amount_x; }
    int get_tile_amount_y() const { return tile_amount_y; }
    Vector3 get_init_position() const { return init_position; }
    float get_wait_time() const { return wait_time; }
   

    // Setters
    void set_tile_size(float size) { tile_size = size; }
    void set_tile_amount_x(int amount) { tile_amount_x = amount; }
    void set_tile_amount_y(int amount) { tile_amount_y = amount; }
    void set_init_position(const Vector3& pos) { init_position = pos; }
    void set_wait_time(float time) { wait_time = time; }

    TileToolCreator(float tile_size = 20.0f, int tile_amount_x = 3, int tile_amount_y = 3, Vector3 init_position = Vector3(-15, 0, -20), float wait_time = 1.0f);
    ~TileToolCreator();
    
    void _process(double delta);
    void _activate_tool();
    void _take_screenshot( Camera3D *camera, int x, int y);

};

} // namespace godot

#endif // SCREENSHOT_EXTENSION_HPP