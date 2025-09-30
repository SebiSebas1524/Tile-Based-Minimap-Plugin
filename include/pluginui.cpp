#include "pluginui.hpp"
#include <godot_cpp/classes/editor_interface.hpp>

void PluginUI::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("_button_pressed"), &PluginUI::_button_pressed);
    ClassDB::bind_method(D_METHOD("_on_tool_finished"), &PluginUI::_on_tool_finished);
    
}

PluginUI::PluginUI()
{
    m_button = memnew(Button);
    m_tool = memnew(TileToolCreator);
 
}

PluginUI::~PluginUI()
{
    if(m_button) memdelete(m_button);
    if(m_tool) memdelete(m_tool);
}

void PluginUI::_enter_tree()
{
    m_button->set_text("My tool button");
    m_button->connect("pressed", Callable(this, "_button_pressed")); // connect your function to the pressed signal
    

    add_child(m_tool);
    add_control_to_dock(EditorPlugin::DOCK_SLOT_LEFT_BL, m_button);

    m_tool->connect("tool_finished", Callable(this, "_on_tool_finished"));
}

void PluginUI::_exit_tree()
{
    remove_control_from_docks(m_button);
    m_button->queue_free();
}

void PluginUI::_button_pressed() {
    m_button->set_disabled(true); // disable immediately
    UtilityFunctions::print("Button has been pressed!");

    if (!m_tool)
    {
        m_tool = memnew(TileToolCreator);
        add_child(m_tool);
    }

    // Start the tool's operation
    m_tool->_activate_tool();

}

void PluginUI::_on_tool_finished() {
    UtilityFunctions::print("Tool finished!");
    m_button->set_disabled(false); // re-enable
}
