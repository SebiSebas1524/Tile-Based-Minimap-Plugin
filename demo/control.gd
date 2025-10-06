extends Control

@export var tile_size_px: int = 512
@export var tile_amount: int = 3 # match your bake settings (3x3 in your screenshot)
@export var tiles_path: String = "res://tiles"

var tile_textures: Dictionary = {}

func _ready():
	load_tiles()
	queue_redraw()

func load_tiles():
	for x in range(tile_amount):
		for y in range(tile_amount):
			var path = "%s/tile_%d_%d.png" % [tiles_path, x, y]
			if ResourceLoader.exists(path):
				var tex: Texture2D = load(path)
				tile_textures[Vector2i(x, y)] = tex

func _draw():
	for pos in tile_textures.keys():
		var tex: Texture2D = tile_textures[pos]
		var screen_pos = Vector2(pos.x * tile_size_px, pos.y * tile_size_px)
		draw_texture(tex, screen_pos)

	# Example: draw player marker
	var player = get_tree().get_first_node_in_group("player")
	if player:
		var marker_pos = world_to_minimap(player.global_position)
		draw_circle(marker_pos, 8, Color.RED)

func world_to_minimap(world_pos: Vector3) -> Vector2:
	# Assuming baked tiles match evenly with world grid
	var rel_x = world_pos.x / float(tile_size_px)
	var rel_y = world_pos.z / float(tile_size_px)
	return Vector2(rel_x, rel_y)
