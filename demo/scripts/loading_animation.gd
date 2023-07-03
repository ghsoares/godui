extends Control

## The UI reference
var ui: UI = null

## The noise
@export var noise: FastNoiseLite

## The number of arms
@export var sections: int = 4

## The number of dots
@export var dots: int = 8

## Dots speed
@export var speed: float = 1.0

## Spacing between each dot
@export var spacing: float = 1.0

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

func arm(nosize: float, offset: float) -> Vector2:
	var p: Vector2 = Vector2()
	for i in range(sections):
		var freqx: float = noise.get_noise_2d(i * 64.0, 0.0) * 0.5 + 0.5
		var freqy: float = noise.get_noise_2d(i * 256.0, 256.0) * 0.5 + 0.5
		freqx = lerp(-8.0, 8.0, freqx)
		freqy = lerp(-8.0, 8.0, freqy)
		var s: float = size / sections
		p += Vector2(
			cos(offset * TAU * freqx) * s,
			sin(offset * TAU * freqy) * s
		)
	return p

## Called to update the interface
func ui_process(ui: UI) -> void:
	var main: UI = ui.add(Control).prop("custom_minimum_size", Vector2(64.0, 64.0))

	main.draw(func (draw):
		var t: float = draw.time
		var node: Control = draw.node
		var size: Vector2 = node.size
		var rad: float = 3.0
		var s: Vector2 = size - Vector2.ONE * rad * 2.0

		for i in range(dots):
			var off: float = t * speed + (i / (dots - 1.0)) * spacing
			var d: Vector2 = arm(min(s.x, s.y) * 0.5, off) + size * 0.5
			var c: Color = Color.BLACK.lerp(Color.RED, i / (dots - 1.0))

			node.draw_circle(d, rad, c)

		draw.redraw()
	)
