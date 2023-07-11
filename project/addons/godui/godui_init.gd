@tool
extends Node

## Current Godui resources instance
var godui_resources_instance: Object = null

## Called when entering tree
func _enter_tree() -> void:
	godui_resources_instance = ResourceLoader.load("res://addons/godui/godui_resources.gd").new()
	Engine.register_singleton("GoduiResources", godui_resources_instance)

## Called when exiting tree
func _exit_tree() -> void:
	Engine.unregister_singleton("GoduiResources")
	godui_resources_instance.free()