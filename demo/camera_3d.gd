extends Camera3D

@export var move_speed: float = 20.0
@export var sprint_multiplier: float = 2.0
@export var mouse_sensitivity: float = 0.002
@export var min_pitch: float = -89.0
@export var max_pitch: float = 89.0

var rotation_x: float = 0.0
var mouse_delta: Vector2 = Vector2.ZERO

func _ready() -> void:
	Input.mouse_mode = Input.MOUSE_MODE_CAPTURED

func _input(event: InputEvent) -> void:
	# Capture mouse movement
	if event is InputEventMouseMotion and Input.mouse_mode == Input.MOUSE_MODE_CAPTURED:
		mouse_delta = event.relative
	
	# Toggle mouse capture with ESC
	if event.is_action_pressed("ui_cancel"):
		if Input.mouse_mode == Input.MOUSE_MODE_CAPTURED:
			Input.mouse_mode = Input.MOUSE_MODE_VISIBLE
		else:
			Input.mouse_mode = Input.MOUSE_MODE_CAPTURED

func _physics_process(delta: float) -> void:
	# Handle mouse look
	if mouse_delta != Vector2.ZERO:
		rotate_y(-mouse_delta.x * mouse_sensitivity)
		rotation_x -= mouse_delta.y * mouse_sensitivity
		rotation_x = clamp(rotation_x, deg_to_rad(min_pitch), deg_to_rad(max_pitch))
		rotation.x = rotation_x
		mouse_delta = Vector2.ZERO
	
	# Get movement input
	var input_dir = Input.get_vector("ui_left", "ui_right", "ui_up", "ui_down")
	var direction = (transform.basis * Vector3(input_dir.x, 0, input_dir.y)).normalized()
	
	# Apply sprint
	var speed = move_speed
	if Input.is_action_pressed("ui_select"):  # Shift
		speed *= sprint_multiplier
	
	# Vertical movement
	if Input.is_action_pressed("ui_page_up"):  # E
		direction.y += 1
	if Input.is_action_pressed("ui_page_down"):  # Q
		direction.y -= 1
	
	# Normalize if moving diagonally with vertical input
	if direction.length() > 1.0:
		direction = direction.normalized()
	
	# Apply movement
	if direction:
		global_position += direction * speed * delta
