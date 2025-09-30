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
    int tile_length = 3;

    const Vector3 init_position = Vector3(-15, 0, -20);

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
    TileToolCreator();
    ~TileToolCreator();
    
    void _process(double delta);
    void _activate_tool();
    void _take_screenshot( Camera3D *camera, int x, int y);
};

} // namespace godot

#endif // SCREENSHOT_EXTENSION_HPP