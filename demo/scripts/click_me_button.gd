extends Control

## The UI reference
var ui: UI = null

# The number of clicks registered by the button
var clicks: int = 0

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

## Called to update the interface
func ui_process(ui: UI) -> void:
	# Add a button
	var button: UI = ui.add(Button)

	# Display a simple text with no clicks
	if clicks == 0:
		button.prop("text", "click me!")
	# Else display the number of clicks
	else:
		# We're using format string to insert the click count inside the text
		button.prop("text", "you clicked %d times!" % clicks)
	
	# Here we're connecting to the button's "pressed" signal 
	button.event("pressed", func (): 
		# Increment "clicks" by 1 every time the signal is emitted
		clicks += 1

		# Requests the UI to update
		ui.queue_update()
	)
