extends Node

func _ready() -> void:
	var value = tbm_plugin.sum([1,2,3,4,5, "test"])

	print(value) # 5.0

	var urls = tbm_plugin.get_key_values([{"name": "Google", "url":"http://google.com"}], "url")

	print(urls)


func _on_screensaver_wall_bounce() -> void:
	print("boop!")
	pass # Replace with function body.
	
