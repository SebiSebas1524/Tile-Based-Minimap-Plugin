@tool
extends Node3D

var immediate_mesh: ImmediateMesh
var mesh_instance: MeshInstance3D

@export var grid_cols: int = 3:
	set(value):
		grid_cols = value
		call_deferred("_draw_grid")

@export var grid_rows: int = 3:
	set(value):
		grid_rows = value
		call_deferred("_draw_grid")

@export var cell_size: float = 1.0:
	set(value):
		cell_size = value
		call_deferred("_draw_grid")

@export var grid_color: Color = Color.GREEN:
	set(value):
		grid_color = value
		call_deferred("_draw_grid")

@export var show_grid: bool = true:
	set(value):
		show_grid = value
		if mesh_instance:
			mesh_instance.visible = value
		call_deferred("_draw_grid")

@export var refresh: bool = false:
	set(value):
		if value:
			_draw_grid()
			refresh = false

func _ready():
	_setup()
	_draw_grid()

func _setup():
	# Remove old mesh if exists
	for child in get_children():
		if child is MeshInstance3D:
			child.queue_free()
	
	mesh_instance = MeshInstance3D.new()
	immediate_mesh = ImmediateMesh.new()
	mesh_instance.mesh = immediate_mesh
	
	var material = StandardMaterial3D.new()
	material.shading_mode = BaseMaterial3D.SHADING_MODE_UNSHADED
	material.albedo_color = grid_color
	material.no_depth_test = true
	mesh_instance.material_override = material
	
	add_child(mesh_instance, true)
	if Engine.is_editor_hint():
		mesh_instance.set_owner(get_tree().edited_scene_root)

func _draw_grid():
	if not is_inside_tree() or not mesh_instance or not immediate_mesh:
		return
	
	immediate_mesh.clear_surfaces()
	
	if not show_grid:
		return
	
	immediate_mesh.surface_begin(Mesh.PRIMITIVE_LINES)
	
	var start_x = -(grid_cols * cell_size) / 2.0
	var start_z = -(grid_rows * cell_size) / 2.0
	
	# Draw vertical lines
	for i in range(grid_cols + 1):
		var x = start_x + i * cell_size
		immediate_mesh.surface_set_color(grid_color)
		immediate_mesh.surface_add_vertex(Vector3(x, 0, start_z))
		immediate_mesh.surface_add_vertex(Vector3(x, 0, start_z + grid_rows * cell_size))
	
	# Draw horizontal lines
	for i in range(grid_rows + 1):
		var z = start_z + i * cell_size
		immediate_mesh.surface_set_color(grid_color)
		immediate_mesh.surface_add_vertex(Vector3(start_x, 0, z))
		immediate_mesh.surface_add_vertex(Vector3(start_x + grid_cols * cell_size, 0, z))
	
	immediate_mesh.surface_end()
	
	# Draw labels
	_draw_labels(start_x, start_z)

func _draw_labels(start_x: float, start_z: float):
	# Remove old labels
	for child in get_children():
		if child is Label3D:
			child.queue_free()
	
	# Add new labels
	for row in range(grid_rows):
		for col in range(grid_cols):
			var label = Label3D.new()
			label.text = str(col) + "," + str(row)
			label.position = Vector3(
				start_x + col * cell_size + cell_size / 2.0,
				0.01,
				start_z + row * cell_size + cell_size / 2.0
			)
			label.billboard = BaseMaterial3D.BILLBOARD_ENABLED
			label.no_depth_test = true
			label.pixel_size = 0.005
			label.font_size = 32
			label.modulate = grid_color
			
			add_child(label, true)
			if Engine.is_editor_hint():
				label.set_owner(get_tree().edited_scene_root)
