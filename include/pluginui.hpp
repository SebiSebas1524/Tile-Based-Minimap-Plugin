#pragma once

#include <godot_cpp/classes/editor_plugin.hpp>
#include <godot_cpp/classes/button.hpp>
#include <tile_tool_creator.hpp>

#include <godot_cpp/classes/sub_viewport.hpp>

using namespace godot;

class PluginUI : public EditorPlugin {
    GDCLASS(PluginUI, EditorPlugin);

protected:
    static void _bind_methods();

private:
    Button *m_button = nullptr;
    TileToolCreator *m_tool = nullptr;
public:
    PluginUI();
    ~PluginUI();

    void _enter_tree() override;
    void _exit_tree() override;
    void _button_pressed();

    void _on_tool_finished();
};