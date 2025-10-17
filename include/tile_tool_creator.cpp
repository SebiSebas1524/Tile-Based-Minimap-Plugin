#include "tile_tool_creator.hpp"

#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/viewport_texture.hpp>
#include <godot_cpp/classes/camera3d.hpp>
#include <godot_cpp/classes/dir_access.hpp>
#include <godot_cpp/classes/editor_interface.hpp>
#include <godot_cpp/classes/editor_file_system.hpp>
#include <godot_cpp/classes/engine.hpp>


using namespace godot;

void TileToolCreator::_bind_methods() {
    ClassDB::bind_method(D_METHOD("_take_screenshot", "sub_vp", "camera"), &TileToolCreator::_take_screenshot);
    ClassDB::bind_method(D_METHOD("_activate_tool"), &TileToolCreator::_activate_tool);
    
    ADD_SIGNAL(MethodInfo("tool_finished"));
}

godot::TileToolCreator::TileToolCreator(float tile_size, int tile_amount_x, int tile_amount_y,
                                        Vector3 init_position, float wait_time) 
                                        : tile_size(tile_size),
                                        tile_amount_x(tile_amount_x), 
                                        tile_amount_y(tile_amount_y),
                                        init_position(init_position),
                                        wait_time(wait_time)
{
    UtilityFunctions::print("Tool for tiles created!");
}

TileToolCreator::~TileToolCreator() {}

void TileToolCreator::_activate_tool() {
    // Create folder for screenshots
    String folder = "res://tiles";
    Ref<DirAccess> da = DirAccess::open("res://");
    if (da.is_valid() && !da->dir_exists(folder)) {
        Error err = da->make_dir_recursive(folder);
        if (err != OK) {
            UtilityFunctions::printerr("Failed to create folder: ", folder);
        } else {
            UtilityFunctions::print("Created folder: ", folder);
        }
    }

    sub_vp = memnew(SubViewport);
    sub_vp->set_size(Vector2i(1200, 1200));
    sub_vp->set_update_mode(SubViewport::UPDATE_ALWAYS);
    sub_vp->set_clear_mode(SubViewport::CLEAR_MODE_ALWAYS);

    add_child(sub_vp);


    camera = memnew(Camera3D);
    camera->set_current(true);
    camera->set_projection(Camera3D::PROJECTION_ORTHOGONAL);
    camera->set_orthogonal(tile_size, 1.2f, 1000.0f);
    camera->set_position(Vector3(init_position.x, 10 , init_position.z));
    camera->set_rotation_degrees(Vector3(-90, 0, 0));
    sub_vp->add_child(camera);

    current_x = 0;
    current_z = 0;
    elapsed = 0.0f;
    capturing = true;

    set_process(true); // enable _process
}

void TileToolCreator::_process(double delta) {
    if (!capturing) return;

    elapsed += delta;
    if (elapsed < wait_time) return;

    elapsed = 0.0f;

    if (!ready_to_capture) {
        camera->set_position(Vector3(init_position.x + (current_x * tile_size),
                                     camera->get_position().y,
                                     init_position.z + (current_z * tile_size)));
        camera->force_update_transform();

        ready_to_capture = true;
        return; 
    }
    _take_screenshot(camera, current_x, current_z);

    ready_to_capture = false; 

    // Advance indices
    current_x++;
    if (current_x > tile_amount_x - 1) {
        current_x = 0;
        current_z++;
    }

    if (current_z > tile_amount_y - 1) {
        capturing = false;
        set_process(false);

        camera->queue_free();
        sub_vp->queue_free();
        emit_signal("tool_finished");
    }
}

void TileToolCreator::_take_screenshot( Camera3D *camera, int x, int y) {
   if (!camera || !camera->get_viewport()) {
        UtilityFunctions::printerr("Camera or viewport is null!");
        return;
    }

    Ref<Image> img = camera->get_viewport()->get_texture()->get_image();
    if (!img.is_valid()) {
        UtilityFunctions::printerr("Failed to get image from viewport.");
        return;
    }

    // Always save into the "tiles" folder
    String file_path = vformat("res://tiles/tile_%d_%d.png", x, y);

    Error err = img->save_png(file_path);
    if (err == OK) {
        UtilityFunctions::print("Saved screenshot to: ", file_path);

        if (godot::Engine::get_singleton()->is_editor_hint()) {
            EditorInterface *editor = EditorInterface::get_singleton();
            if (editor) {
                editor->get_resource_filesystem()->scan();
            }
        }
    
    } else {
        UtilityFunctions::printerr("Failed to save screenshot: ", file_path);
    }
}
