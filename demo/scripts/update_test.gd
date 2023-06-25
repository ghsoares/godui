extends Control

## The UI reference
var ui: UI = null

var nodes: Dictionary = {}

## Called when the node is ready
func _ready() -> void:
	# Create the UI reference, then bind it to self
	ui = UI.create(self)

	# Enable processing interface every frame
	set_process(true)

	for i in range(3):
		nodes[i] = "Node %s" % i
	
	await get_tree().create_timer(5.0).timeout

	nodes.erase(1)

	ui.queue_update()

## Called when receiving a notification
func _notification(what: int) -> void:
	# Notify the interface from the Node's notification
	if ui: ui.notification(what)

## Called to update the interface
func ui_process(ui: UI) -> void:
	var list: UI = ui.add(VBoxContainer).margin(8.0)

	for k in nodes.keys():
		var n = nodes[k]
		var cont: UI = list.add(HBoxContainer, k)
		var l: UI = cont.add(Label)
		var n_id: int = l.ref().get_instance_id()
		l.prop("text", "%s: ID %s" % [n, n_id])
