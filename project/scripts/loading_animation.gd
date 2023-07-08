extends Control

## The UI reference
var ui: UI = null

## Called when the node is ready
func _ready() -> void:
	# Create the UI reference, then bind it to self
	ui = UI.create(self)

	# Enable processing interface every frame
	set_process(true)

## Called when receiving a notification
func _notification(what: int) -> void:
	# Notify the interface from the Node's notification
	if ui: ui.notification(what)

## Draws an arc with caps
func draw_capped_arc(node: CanvasItem, center: Vector2, radius: float, start: float, end: float, point_count: int, color: Color, width: float) -> void:
	node.draw_arc(center, radius, start, end, point_count, color, width, true)
	node.draw_circle(center + Vector2(radius, 0.0).rotated(end), width * 0.5, color)
	node.draw_circle(center + Vector2(radius, 0.0).rotated(start), width * 0.5, color)

## Called to update the interface
func ui_process(ui: UI) -> void:
	# Add a simple control to contain the loading animation
	var main: UI = ui.add(Control).prop("custom_minimum_size", Vector2(128.0, 128.0))

	# "draw" allows to directly draw something in the node
	main.draw(func (draw):
		# Get current frame time
		var time: float = draw.time

		# Get the target node reference
		var node: Control = draw.node

		# Get the size of the control
		var size: Vector2 = node.size

		# Get the radius of the arc
		var rad: float = min(size.x, size.y) * 0.5

		# Animate arc start and end angles
		var a0: float = sin(time * TAU / 5.0) * deg_to_rad(90.0)
		var a1: float = cos(time * TAU / 5.0) * deg_to_rad(90.0)

		# a0 should be less than a1
		if a1 < a0:
			var t: float = a1
			a1 = a0
			a0 = t

		# Animate arc rotation
		var rot: float = time * TAU / 1.0

		# Draw an arc with both caps
		draw_capped_arc(node, size * 0.5, rad - 4.0, rot + a0, rot + a1, 32, Color.RED, 8.0)
		
		# Call "redraw" to request to draw again
		draw.redraw()
	)
