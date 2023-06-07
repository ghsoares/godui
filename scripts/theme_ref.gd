extends RefCounted
class_name StyleRef

## The node reference
var __node: Node

## Current stylebox
var __style: StyleBox

## Called to initialize
func _init(node: Node) -> void:
	__node = node

## Modify a empty StyleBox
func empty(name: String, modify_callable: Callable) -> StyleRef:
	# Get stylebox
	var style: StyleBoxEmpty = __node.get_theme_stylebox(name) as StyleBoxEmpty

	# Couldn't find
	if style == null:
		style = StyleBoxEmpty.new()
		__node.add_theme_stylebox_override(name, style)

	# Set self style
	__style = style

	# Call callable
	modify_callable.call(self)
	
	# Chain call
	return self

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

## Set left margin
func left_margin(val: float) -> StyleRef:
	assert(__style)

	# Set margin
	__style.content_margin_left = val

	# Chain call
	return self

## Set top margin
func top_margin(val: float) -> StyleRef:
	assert(__style)

	# Set margin
	__style.content_margin_top = val

	# Chain call
	return self

## Set right margin
func right_margin(val: float) -> StyleRef:
	assert(__style)

	# Set margin
	__style.content_margin_right = val

	# Chain call
	return self

## Set bottom margin
func bottom_margin(val: float) -> StyleRef:
	assert(__style)

	# Set margin
	__style.content_margin_bottom = val

	# Chain call
	return self

## Set horizontal margin
func horizontal_margin(val: float) -> StyleRef:
	assert(__style)

	# Set margin
	__style.content_margin_left = val
	__style.content_margin_right = val

	# Chain call
	return self

## Set vertical margin
func vertical_margin(val: float) -> StyleRef:
	assert(__style)

	# Set margin
	__style.content_margin_top = val
	__style.content_margin_bottom = val

	# Chain call
	return self

## Set all margin
func margin(val: float) -> StyleRef:
	assert(__style)

	# Set margin
	__style.content_margin_left = val
	__style.content_margin_top = val
	__style.content_margin_right = val
	__style.content_margin_bottom = val

	# Chain call
	return self
