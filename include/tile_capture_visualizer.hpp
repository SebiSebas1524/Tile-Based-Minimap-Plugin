#ifndef TILE_CAPTURE_VISUALIZER_HPP
#define TILE_CAPTURE_VISUALIZER_HPP

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/editor_node3d_gizmo_plugin.hpp>
#include <godot_cpp/classes/editor_node3d_gizmo.hpp>
#include <godot_cpp/classes/editor_inspector_plugin.hpp>
#include <godot_cpp/classes/editor_property.hpp>
#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/mesh.hpp>
#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/camera3d.hpp>
#include <godot_cpp/variant/color.hpp>

namespace godot {

class TileCaptureVisualizer : public Node3D {
    GDCLASS(TileCaptureVisualizer, Node3D)

private:
    float tile_size;
    int tile_amount_x;
    int tile_amount_y;
    Vector3 init_position;
    float box_height;
    
    Color grid_color;
    Color box_color;

protected:
    static void _bind_methods();

public:
    TileCaptureVisualizer();
    ~TileCaptureVisualizer();

    void _notification(int p_what);

    void _on_tool_finished();

    // Actions
    void generate_tiles();
    void reset_to_defaults();
    
    // Setters and getters
    void set_tile_size(float size);
    float get_tile_size() const;
    
    void set_tile_amount_x(int amount);
    int get_tile_amount_x() const;
    
    void set_tile_amount_y(int amount);
    int get_tile_amount_y() const;
    
    void set_init_position(Vector3 pos);
    Vector3 get_init_position() const;
    
    void set_box_height(float height);
    float get_box_height() const;
    
    void set_grid_color(Color color);
    Color get_grid_color() const;
    
    void set_box_color(Color color);
    Color get_box_color() const;
};

// ============================================================================
// Gizmo Plugin for the visualizer
class TileCaptureVisualizerGizmoPlugin : public EditorNode3DGizmoPlugin {
    GDCLASS(TileCaptureVisualizerGizmoPlugin, EditorNode3DGizmoPlugin)

protected:
    static void _bind_methods();

public:
    TileCaptureVisualizerGizmoPlugin();
    
    bool _has_gizmo(Node3D* node) const override;
    String _get_gizmo_name() const override;
    int32_t _get_priority() const;
    void _redraw(const Ref<EditorNode3DGizmo> &gizmo) override;

    String _get_handle_name(const Ref<EditorNode3DGizmo>& gizmo, int32_t handle_id, bool secondary) const override;
    Variant _get_handle_value(const Ref<EditorNode3DGizmo>& gizmo, int32_t handle_id, bool secondary) const override;
    void _set_handle(const Ref<EditorNode3DGizmo>& gizmo, int32_t handle_id, bool secondary, Camera3D* camera, const Vector2& screen_pos) override;
    void _commit_handle(const Ref<EditorNode3DGizmo>& gizmo, int32_t handle_id, bool secondary, const Variant& restore, bool cancel) override;
};

// ============================================================================
// Inspector Plugin for custom buttons
class TileCaptureVisualizerInspectorPlugin : public EditorInspectorPlugin {
    GDCLASS(TileCaptureVisualizerInspectorPlugin, EditorInspectorPlugin)

protected:
    static void _bind_methods();

private:
    Vector<Button*> buttons; // Store buttons to enable/disable them during operations

public:
    TileCaptureVisualizerInspectorPlugin();
    
    bool _can_handle(Object* object) const override;
    void _parse_begin(Object* object) override;

    void _on_operation_started(Button *active_btn);
    void _on_operation_finished(Button *active_btn);
};

} // namespace godot

#endif // TILE_CAPTURE_VISUALIZER_HPP