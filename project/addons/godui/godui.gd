@tool
extends EditorPlugin

## Called when entering tree
func _enter_tree() -> void:
	add_autoload_singleton("GoduiInit", "res://addons/godui/godui_init.gd")

## Called when exiting tree
func _exit_tree() -> void:
	remove_autoload_singleton("GoduiInit")