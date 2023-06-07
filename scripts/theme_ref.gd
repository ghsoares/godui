extends RefCounted
class_name StyleRef

## The node reference
var __node: Node

## Current style
var __style: StyleBox

## Called to initialize
func _init(node: Node) -> void:
	__node = node

## Modify a flat StyleBox
func flat(name: String, modify_callable: Callable) -> StyleRef:
	# Get stylebox
	var style: StyleBoxFlat = __node.get_theme_stylebox(name) as StyleBoxFlat

	# Couldn't find
	if style == null:
		style = StyleBoxFlat.new()
		__node.add_theme_stylebox_override(name, style)

	# Set self style
	__style = style

	# Call callable
	modify_callable.call(self)
	
	# Chain call
	return self

