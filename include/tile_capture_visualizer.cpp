#include "tile_capture_visualizer.hpp"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/standard_material3d.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/engine.hpp>
#include "tile_tool_creator.hpp"

using namespace godot;

TileCaptureVisualizer::TileCaptureVisualizer() :
    tile_size(20.0f),
    tile_amount_x(3),
    tile_amount_y(3),
    init_position(Vector3(-15, 0, -20)),
    box_height(20.0f),
    grid_color(Color(0, 1, 0, 0.8)),
    box_color(Color(0.5, 0.5, 1.0, 0.5)) {
}

TileCaptureVisualizer::~TileCaptureVisualizer() {
}

void TileCaptureVisualizer::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_tile_size", "size"), &TileCaptureVisualizer::set_tile_size);
    ClassDB::bind_method(D_METHOD("get_tile_size"), &TileCaptureVisualizer::get_tile_size);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "tile_size"), "set_tile_size", "get_tile_size");
    
    ClassDB::bind_method(D_METHOD("set_tile_amount_x", "amount"), &TileCaptureVisualizer::set_tile_amount_x);
    ClassDB::bind_method(D_METHOD("get_tile_amount_x"), &TileCaptureVisualizer::get_tile_amount_x);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "tile_amount_x", PROPERTY_HINT_RANGE, "1,20,1"), "set_tile_amount_x", "get_tile_amount_x");
    
    ClassDB::bind_method(D_METHOD("set_tile_amount_y", "amount"), &TileCaptureVisualizer::set_tile_amount_y);
    ClassDB::bind_method(D_METHOD("get_tile_amount_y"), &TileCaptureVisualizer::get_tile_amount_y);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "tile_amount_y", PROPERTY_HINT_RANGE, "1,20,1"), "set_tile_amount_y", "get_tile_amount_y");
    
    ClassDB::bind_method(D_METHOD("set_init_position", "pos"), &TileCaptureVisualizer::set_init_position);
    ClassDB::bind_method(D_METHOD("get_init_position"), &TileCaptureVisualizer::get_init_position);
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "init_position"), "set_init_position", "get_init_position");
    
    ClassDB::bind_method(D_METHOD("set_box_height", "height"), &TileCaptureVisualizer::set_box_height);
    ClassDB::bind_method(D_METHOD("get_box_height"), &TileCaptureVisualizer::get_box_height);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "box_height"), "set_box_height", "get_box_height");
    
    ClassDB::bind_method(D_METHOD("set_grid_color", "color"), &TileCaptureVisualizer::set_grid_color);
    ClassDB::bind_method(D_METHOD("get_grid_color"), &TileCaptureVisualizer::get_grid_color);
    ADD_PROPERTY(PropertyInfo(Variant::COLOR, "grid_color"), "set_grid_color", "get_grid_color");
    
    ClassDB::bind_method(D_METHOD("set_box_color", "color"), &TileCaptureVisualizer::set_box_color);
    ClassDB::bind_method(D_METHOD("get_box_color"), &TileCaptureVisualizer::get_box_color);
    ADD_PROPERTY(PropertyInfo(Variant::COLOR, "box_color"), "set_box_color", "get_box_color");
    
    ClassDB::bind_method(D_METHOD("generate_tiles"), &TileCaptureVisualizer::generate_tiles);
    ClassDB::bind_method(D_METHOD("reset_to_defaults"), &TileCaptureVisualizer::reset_to_defaults);

    ClassDB::bind_method(D_METHOD("_on_tool_finished"), &TileCaptureVisualizer::_on_tool_finished);

    ADD_SIGNAL(MethodInfo("operation_started"));
    ADD_SIGNAL(MethodInfo("operation_finished"));
}

void TileCaptureVisualizer::_notification(int p_what) {
    switch (p_what) {
        case NOTIFICATION_READY:
            set_notify_transform(true);
            init_position = get_global_position();
            update_gizmos();
            break;
        case NOTIFICATION_TRANSFORM_CHANGED:
        case NOTIFICATION_LOCAL_TRANSFORM_CHANGED:
            init_position = get_global_position();
            update_gizmos();
            break;
    }
}

void TileCaptureVisualizer::_on_tool_finished() {
    UtilityFunctions::print("TileCaptureVisualizer: Tool finished!");
    
    for (int i = 0; i < get_child_count(); i++) {
        TileToolCreator* tool = Object::cast_to<TileToolCreator>(get_child(i));
        if (tool) {
            tool->queue_free(); 
            break;
        }
    }
    
    emit_signal("operation_finished");
}

void TileCaptureVisualizer::generate_tiles() {
    UtilityFunctions::print("TileCaptureVisualizer: Generate tiles!");
    
    emit_signal("operation_started");

    TileToolCreator* tool = memnew(TileToolCreator(get_tile_size(), get_tile_amount_x(), 
                                                     get_tile_amount_y(), get_init_position(), 0.5f));
    add_child(tool);
    
    tool->connect("tool_finished", Callable(this, "_on_tool_finished"));
    
    tool->_activate_tool();
}

void TileCaptureVisualizer::reset_to_defaults() {
    UtilityFunctions::print("TileCaptureVisualizer: Reset to defaults!");
    
    emit_signal("operation_started");
    
    set_tile_size(20.0f);
    set_tile_amount_x(3);
    set_tile_amount_y(3);
    set_init_position(Vector3(0, 0, 0));
    set_box_height(20.0f);
    
    emit_signal("operation_finished");
}

// Setters and getters
void TileCaptureVisualizer::set_tile_size(float size) {
    tile_size = size;
    update_gizmos();
}

float TileCaptureVisualizer::get_tile_size() const {
    return tile_size;
}

void TileCaptureVisualizer::set_tile_amount_x(int amount) {
    tile_amount_x = amount;
    update_gizmos();
}

int TileCaptureVisualizer::get_tile_amount_x() const {
    return tile_amount_x;
}

void TileCaptureVisualizer::set_tile_amount_y(int amount) {
    tile_amount_y = amount;
    update_gizmos();
}

int TileCaptureVisualizer::get_tile_amount_y() const {
    return tile_amount_y;
}

void TileCaptureVisualizer::set_init_position(Vector3 pos) {
    init_position = pos;
    set_global_position(pos); 
}

Vector3 TileCaptureVisualizer::get_init_position() const {
    return init_position;
}

void TileCaptureVisualizer::set_box_height(float height) {
    box_height = height;
    update_gizmos();
}

float TileCaptureVisualizer::get_box_height() const {
    return box_height;
}

void TileCaptureVisualizer::set_grid_color(Color color) {
    grid_color = color;
    update_gizmos();
}

Color TileCaptureVisualizer::get_grid_color() const {
    return grid_color;
}

void TileCaptureVisualizer::set_box_color(Color color) {
    box_color = color;
    update_gizmos();
}

Color TileCaptureVisualizer::get_box_color() const {
    return box_color;
}

// ============================================================================
// Gizmo Plugin Implementation
TileCaptureVisualizerGizmoPlugin::TileCaptureVisualizerGizmoPlugin() {
    create_material("grid_material", Color(0, 1, 0, 0.8));
    create_material("box_material", Color(0.5, 0.5, 1.0, 0.5));
    create_handle_material("handles");
}

void TileCaptureVisualizerGizmoPlugin::_bind_methods() {
}

bool TileCaptureVisualizerGizmoPlugin::_has_gizmo(Node3D* node) const {
    return Object::cast_to<TileCaptureVisualizer>(node) != nullptr;
}

String TileCaptureVisualizerGizmoPlugin::_get_gizmo_name() const {
    return "TileCaptureVisualizer";
}

int32_t TileCaptureVisualizerGizmoPlugin::_get_priority() const {
    return -1;
}

void TileCaptureVisualizerGizmoPlugin::_redraw(const Ref<EditorNode3DGizmo>& gizmo) {
    gizmo->clear();
    
    TileCaptureVisualizer* visualizer = Object::cast_to<TileCaptureVisualizer>(gizmo->get_node_3d());
    if (!visualizer) return;
    
    float tile_size = visualizer->get_tile_size();
    int tile_amount_x = visualizer->get_tile_amount_x();
    int tile_amount_y = visualizer->get_tile_amount_y();
    float box_height = visualizer->get_box_height();
    
    Vector3 local_center = Vector3(0, 0, 0); 
    
    PackedVector3Array grid_lines;
    PackedVector3Array box_lines;
    
    // Grid starts at TOP-LEFT corner of tile [0,0] relative to node
    float grid_start_x = local_center.x - (tile_size / 2.0f);
    float grid_start_z = local_center.z - (tile_size / 2.0f);
    
    float grid_end_x = grid_start_x + (tile_amount_x * tile_size);
    float grid_end_z = grid_start_z + (tile_amount_y * tile_size);
    
    float grid_y = local_center.y;
    
    // Draw GRID on top (flat, horizontal)
    for (int x = 0; x <= tile_amount_x; x++) {
        float x_pos = grid_start_x + (x * tile_size);
        grid_lines.push_back(Vector3(x_pos, grid_y, grid_start_z));
        grid_lines.push_back(Vector3(x_pos, grid_y, grid_end_z));
    }
    
    // Horizontal lines (along Z axis) 
    for (int y = 0; y <= tile_amount_y; y++) {
        float z_pos = grid_start_z + (y * tile_size);
        grid_lines.push_back(Vector3(grid_start_x, grid_y, z_pos));
        grid_lines.push_back(Vector3(grid_end_x, grid_y, z_pos));
    }
    
    // Draw BOX wireframe (bottom box showing capture volume)
    float min_x = grid_start_x;
    float max_x = grid_end_x;
    float min_z = grid_start_z;
    float max_z = grid_end_z;
    float min_y = local_center.y - box_height;
    float max_y = local_center.y;
    
    // Bottom face
    box_lines.push_back(Vector3(min_x, min_y, min_z));
    box_lines.push_back(Vector3(max_x, min_y, min_z));
    
    box_lines.push_back(Vector3(max_x, min_y, min_z));
    box_lines.push_back(Vector3(max_x, min_y, max_z));
    
    box_lines.push_back(Vector3(max_x, min_y, max_z));
    box_lines.push_back(Vector3(min_x, min_y, max_z));
    
    box_lines.push_back(Vector3(min_x, min_y, max_z));
    box_lines.push_back(Vector3(min_x, min_y, min_z));
    
    // Top face (at grid level)
    box_lines.push_back(Vector3(min_x, max_y, min_z));
    box_lines.push_back(Vector3(max_x, max_y, min_z));
    
    box_lines.push_back(Vector3(max_x, max_y, min_z));
    box_lines.push_back(Vector3(max_x, max_y, max_z));
    
    box_lines.push_back(Vector3(max_x, max_y, max_z));
    box_lines.push_back(Vector3(min_x, max_y, max_z));
    
    box_lines.push_back(Vector3(min_x, max_y, max_z));
    box_lines.push_back(Vector3(min_x, max_y, min_z));
    
    // Vertical edges
    box_lines.push_back(Vector3(min_x, min_y, min_z));
    box_lines.push_back(Vector3(min_x, max_y, min_z));
    
    box_lines.push_back(Vector3(max_x, min_y, min_z));
    box_lines.push_back(Vector3(max_x, max_y, min_z));
    
    box_lines.push_back(Vector3(max_x, min_y, max_z));
    box_lines.push_back(Vector3(max_x, max_y, max_z));
    
    box_lines.push_back(Vector3(min_x, min_y, max_z));
    box_lines.push_back(Vector3(min_x, max_y, max_z));
    
    // Add lines to gizmo
    gizmo->add_lines(grid_lines, get_material("grid_material", gizmo));
    gizmo->add_lines(box_lines, get_material("box_material", gizmo));
    
    // Add handles for resizing (in local space)
    PackedVector3Array handles;
    // Handle 0: tile_amount_x (right side of grid)
    handles.push_back(Vector3(grid_end_x, grid_y, local_center.z));
    // Handle 1: tile_amount_y (back side of grid)
    handles.push_back(Vector3(local_center.x, grid_y, grid_end_z));
    // Handle 2: box_height (bottom center)
    handles.push_back(Vector3(local_center.x, min_y, local_center.z));
    
    PackedInt32Array handle_ids;
    handle_ids.push_back(0);
    handle_ids.push_back(1);
    handle_ids.push_back(2);
    
    gizmo->add_handles(handles, get_material("handles", gizmo), handle_ids, false, false);
}

String TileCaptureVisualizerGizmoPlugin::_get_handle_name(const Ref<EditorNode3DGizmo>& gizmo, int32_t handle_id, bool secondary) const {
    switch (handle_id) {
        case 0: return "Tile Amount X";
        case 1: return "Tile Amount Y";
        case 2: return "Box Height";
        default: return "";
    }
}

// ============================================================================
// Inspector Plugin Implementation
TileCaptureVisualizerInspectorPlugin::TileCaptureVisualizerInspectorPlugin() {
}

void TileCaptureVisualizerInspectorPlugin::_bind_methods() {
    ClassDB::bind_method(D_METHOD("_on_operation_started", "active_btn"), 
                        &TileCaptureVisualizerInspectorPlugin::_on_operation_started);
    ClassDB::bind_method(D_METHOD("_on_operation_finished", "active_btn"), 
                        &TileCaptureVisualizerInspectorPlugin::_on_operation_finished);
}

bool TileCaptureVisualizerInspectorPlugin::_can_handle(Object* object) const {
    return Object::cast_to<TileCaptureVisualizer>(object) != nullptr;
}

void TileCaptureVisualizerInspectorPlugin::_parse_begin(Object* object) {
    TileCaptureVisualizer* visualizer = Object::cast_to<TileCaptureVisualizer>(object);
    if (!visualizer) return;
    
    Button* generate_btn = memnew(Button);
    generate_btn->set_text("Generate Tiles");
    generate_btn->connect("pressed", Callable(visualizer, "generate_tiles"));
    add_custom_control(generate_btn);
    
    Button* reset_btn = memnew(Button);
    reset_btn->set_text("Reset to Defaults");
    reset_btn->connect("pressed", Callable(visualizer, "reset_to_defaults"));
    add_custom_control(reset_btn);
    
    // Connect signals and bind the specific button
    visualizer->connect("operation_started", Callable(this, "_on_operation_started").bind(generate_btn));
    visualizer->connect("operation_finished", Callable(this, "_on_operation_finished").bind(generate_btn));
    
    // Store all buttons for disabling/enabling them all
    buttons.push_back(generate_btn);
    buttons.push_back(reset_btn);
}

void TileCaptureVisualizerInspectorPlugin::_on_operation_started(Button* active_btn) {
    // Disable ALL buttons
    for (Button* btn : buttons) {
        btn->set_disabled(true);
    }
    
    // Only change text on the button that was pressed
    active_btn->set_text("Loading...");
}

void TileCaptureVisualizerInspectorPlugin::_on_operation_finished(Button* active_btn) {
    // Enable ALL buttons
    for (Button* btn : buttons) {
        btn->set_disabled(false);
    }
    
    // Restore text on the button that was pressed
    active_btn->set_text("Generate Tiles"); // Or store original text
}

Variant TileCaptureVisualizerGizmoPlugin::_get_handle_value(const Ref<EditorNode3DGizmo>& gizmo, int32_t handle_id, bool secondary) const {
    TileCaptureVisualizer* visualizer = Object::cast_to<TileCaptureVisualizer>(gizmo->get_node_3d());
    if (!visualizer) return Variant();
    
    switch (handle_id) {
        case 0: return visualizer->get_tile_amount_x();
        case 1: return visualizer->get_tile_amount_y();
        case 2: return visualizer->get_box_height();
        default: return Variant();
    }
}

void TileCaptureVisualizerGizmoPlugin::_set_handle(const Ref<EditorNode3DGizmo>& gizmo, int32_t handle_id, bool secondary, Camera3D* camera, const Vector2& screen_pos) {
    TileCaptureVisualizer* visualizer = Object::cast_to<TileCaptureVisualizer>(gizmo->get_node_3d());
    if (!visualizer || !camera) return;
    
    // Get viewport to calculate rays
    Viewport* viewport = camera->get_viewport();
    if (!viewport) return;
    
    Transform3D camera_transform = camera->get_global_transform();
    Vector2 viewport_size = viewport->get_visible_rect().size;
    
    // Calculate ray manually since we can't call project_ray methods
    Vector3 ray_from = camera_transform.origin;
    Vector3 ray_dir;
    
    if (camera->get_projection() == Camera3D::PROJECTION_PERSPECTIVE) {
        // Calculate direction for perspective camera
        Vector2 screen_point = screen_pos;
        Vector2 ndc = (screen_point / viewport_size) * 2.0 - Vector2(1, 1);
        ndc.y = -ndc.y;
        
        float fov = camera->get_fov();
        float aspect = viewport_size.x / viewport_size.y;
        float tan_fov = Math::tan(Math::deg_to_rad(fov * 0.5f));
        
        Vector3 view_dir = camera_transform.basis.get_column(2);
        Vector3 view_right = camera_transform.basis.get_column(0);
        Vector3 view_up = camera_transform.basis.get_column(1);
        
        ray_dir = (-view_dir + view_right * ndc.x * aspect * tan_fov + view_up * ndc.y * tan_fov).normalized();
    } else {
        // Orthogonal projection
        ray_dir = -camera_transform.basis.get_column(2).normalized();
    }
    
    float tile_size = visualizer->get_tile_size();
    Vector3 node_pos = visualizer->get_global_position();
    
    // Grid starts at node position - tile_size/2 (in world space)
    float grid_start_x = node_pos.x - (tile_size / 2.0f);
    float grid_start_z = node_pos.z - (tile_size / 2.0f);
    
    if (handle_id == 0) { // tile_amount_x
        Plane plane(Vector3(0, 1, 0), node_pos.y);
        Vector3 intersection;
        if (plane.intersects_ray(ray_from, ray_dir, &intersection)) {
            float offset = intersection.x - grid_start_x;
            int new_amount = Math::round(offset / tile_size);
            if (new_amount < 1) new_amount = 1;
            if (new_amount > 20) new_amount = 20;
            visualizer->set_tile_amount_x(new_amount);
        }
    } else if (handle_id == 1) { // tile_amount_y
        Plane plane(Vector3(0, 1, 0), node_pos.y);
        Vector3 intersection;
        if (plane.intersects_ray(ray_from, ray_dir, &intersection)) {
            float offset = intersection.z - grid_start_z;
            int new_amount = Math::round(offset / tile_size);
            if (new_amount < 1) new_amount = 1;
            if (new_amount > 20) new_amount = 20;
            visualizer->set_tile_amount_y(new_amount);
        }
    } else if (handle_id == 2) { // box_height
        Plane plane(Vector3(1, 0, 0), node_pos.x);
        Vector3 intersection;
        if (plane.intersects_ray(ray_from, ray_dir, &intersection)) {
            float new_height = node_pos.y - intersection.y;
            if (new_height < 1.0f) new_height = 1.0f;
            if (new_height > 100.0f) new_height = 100.0f;
            visualizer->set_box_height(new_height);
        }
    }
}

void TileCaptureVisualizerGizmoPlugin::_commit_handle(const Ref<EditorNode3DGizmo>& gizmo, int32_t handle_id, bool secondary, const Variant& restore, bool cancel) {
    TileCaptureVisualizer* visualizer = Object::cast_to<TileCaptureVisualizer>(gizmo->get_node_3d());
    if (!visualizer) return;
    
    if (cancel) {
        switch (handle_id) {
            case 0: visualizer->set_tile_amount_x(restore); break;
            case 1: visualizer->set_tile_amount_y(restore); break;
            case 2: visualizer->set_box_height(restore); break;
        }
    }
}