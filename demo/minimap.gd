extends Minimap

@onready var minimap: Minimap = $Minimap 

func _ready(): 
	minimap.load_tiles_from_folder("res://tiles")
