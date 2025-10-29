extends BlipManager

func _ready():
	create_color_blip(Vector2(1,1),Color.AQUA,1.0)
	create_color_blip(Vector2(-2,1),Color.REBECCA_PURPLE,2.0)
	var player_icon = load("res://icon.svg")
	create_icon_blip(Vector2(-10, -10), player_icon, 8.0)
