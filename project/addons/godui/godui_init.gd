@tool
extends Node

## Called when entering the tree
func _enter_tree() -> void:
	UI.set_builtin_classes(get_control_class_scripts())

## Called to get the native script for each Control-based node in Godot
func get_control_class_scripts() -> Dictionary[StringName, Object]:
	# Get the class names
	var classes: PackedStringArray
	classes.append("Control")
	classes.append_array(ClassDB.get_inheriters_from_class("Control"))

	# Create the script code
	var source_code: String = "extends RefCounted\nvar scripts_dict: Dictionary[StringName, Object] = {"
	for c: String in classes:
		if not ClassDB.can_instantiate(c): continue
		source_code += "\n\t&\"%s\": %s," % [c, c]
	source_code += "\n}"

	# Create the script
	var script: GDScript = GDScript.new()
	script.source_code = source_code
	var err: int = script.reload()
	if err != OK: return {}

	# Create the instance and return the scripts
	var ins = script.new()
	return ins.scripts_dict