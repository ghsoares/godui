extends RefCounted
class_name StyleRef

## The node reference
var __node: Node

## Current node type
var __type: String

## Current styleboxes
var __styles: Array[StyleBox]

## Current theme
var __theme: Theme

## Is global or local?
var __local: bool

## Called to initialize
func _init(node: Node, local: bool) -> void:
	__node = node
	__local = local
	__type = node.get_class()

## Get theme
func theme() -> Theme:
	# Get theme
	if __theme == null:
		# From node
		__theme = __node.theme

		# Or create one
		if __theme == null:
			__theme = Theme.new()

			# And set to node
			__node.theme = __theme
	
	return __theme

## Modify style with type
func with_type(type_name: String, modify_callable: Callable) -> StyleRef:
	# Only work on global mode
	assert(not __local)

	# Previous type
	var t: String = __type

	# Set type
	__type = type_name

	# Call callable
	modify_callable.call(self)

	# Set previous type back
	__type = t

	return self

## Modify style with variation
func with_variation(variation_name: String, modify_callable: Callable) -> StyleRef:
	# Only work on global mode
	assert(not __local)

	# Previous type
	var t: String = __type

	# Set variation
	if not theme().is_type_variation(variation_name, __type):
		theme().add_type(variation_name)
		theme().set_type_variation(variation_name, __type)
		

	# Set type
	__type = variation_name

	# Call callable
	modify_callable.call(self)

	# Set previous type back
	__type = t

	return self

## Get stylebox
func get_stylebox(name: String) -> StyleBox:
	# Local mode, must have override
	if __local:
		if not __node.has_theme_stylebox_override(name): 
			return null
		else:
			return __node.get_theme_stylebox(name, __type)
	else:
		if not theme().has_stylebox(name, __type):
			return null
		else:
			return theme().get_stylebox(name, __type)

## Set stylebox
func set_stylebox(name: String, stylebox: StyleBox) -> StyleRef:
	if __local:
		__node.add_theme_stylebox_override(name, stylebox)
	else:
		theme().set_stylebox(name, __type, stylebox)
	
	return self

## Modify a empty StyleBox
func empty(names, modify_callable: Callable) -> StyleRef:
	# Must be a string or an array of strings
	assert(names is String or names is Array)

	# Convert names to array
	if names is String:
		names = [names]
	
	# Set self styleboxes
	__styles.assign(names.map(func (name):
		var style: StyleBoxEmpty = get_stylebox(name) as StyleBoxEmpty

		# Couldn't find
		if style == null:
			style = StyleBoxEmpty.new()
			set_stylebox(name, style)
		
		return style
	))
		
	# Call callable
	modify_callable.call(self)

	# Set styles to empty
	__styles = []
	
	# Chain call
	return self

## Modify a flat StyleBox
func flat(names, modify_callable: Callable) -> StyleRef:
	# Must be a string or an array of strings
	assert(names is String or names is Array)

	# Convert names to array
	if names is String:
		names = [names]
	
	# Set self styleboxes
	__styles.assign(names.map(func (name):
		var style: StyleBoxFlat = get_stylebox(name) as StyleBoxFlat

		# Couldn't find
		if style == null:
			style = StyleBoxFlat.new()
			set_stylebox(name, style)
		
		return style
	))
		
	# Call callable
	modify_callable.call(self)

	# Set styles to empty
	__styles = []
	
	# Chain call
	return self

## Set left margin
func left_margin(val: float) -> StyleRef:
	for __style in __styles:
		assert(__style)

		# Set margin
		__style.content_margin_left = val

	# Chain call
	return self

## Set top margin
func top_margin(val: float) -> StyleRef:
	for __style in __styles:
		assert(__style)

		# Set margin
		__style.content_margin_top = val

	# Chain call
	return self

## Set right margin
func right_margin(val: float) -> StyleRef:
	for __style in __styles:
		assert(__style)

		# Set margin
		__style.content_margin_right = val

	# Chain call
	return self

## Set bottom margin
func bottom_margin(val: float) -> StyleRef:
	for __style in __styles:
		assert(__style)

		# Set margin
		__style.content_margin_bottom = val

	# Chain call
	return self

## Set horizontal margin
func horizontal_margin(val: float) -> StyleRef:
	for __style in __styles:
		assert(__style)

		# Set margin
		__style.content_margin_left = val
		__style.content_margin_right = val

	# Chain call
	return self

## Set vertical margin
func vertical_margin(val: float) -> StyleRef:
	for __style in __styles:
		assert(__style)

		# Set margin
		__style.content_margin_top = val
		__style.content_margin_bottom = val

	# Chain call
	return self

## Set all margin
func margin(val: float) -> StyleRef:
	for __style in __styles:
		assert(__style)

		# Set margin
		__style.content_margin_left = val
		__style.content_margin_top = val
		__style.content_margin_right = val
		__style.content_margin_bottom = val

	# Chain call
	return self

## Set background color
func background_color(color: Color) -> StyleRef:
	for __style in __styles:
		assert(__style and __style is StyleBoxFlat)

		# Set background color
		__style.bg_color = color

	# Chain call
	return self

## Set border color
func border_color(color: Color) -> StyleRef:
	for __style in __styles:
		assert(__style and __style is StyleBoxFlat)

		# Set border color
		__style.border_color = color

	# Chain call
	return self

## Set left border width
func left_border(val: float) -> StyleRef:
	for __style in __styles:
		assert(__style)

		# Set border width
		__style.border_width_left = val

	# Chain call
	return self

## Set top border width
func top_border(val: float) -> StyleRef:
	for __style in __styles:
		assert(__style)

		# Set border width
		__style.border_width_top = val

	# Chain call
	return self

## Set right border width
func right_border(val: float) -> StyleRef:
	for __style in __styles:
		assert(__style)

		# Set border width
		__style.border_width_right = val

	# Chain call
	return self

## Set bottom border width
func bottom_border(val: float) -> StyleRef:
	for __style in __styles:
		assert(__style)

		# Set border width
		__style.border_width_bottom = val

	# Chain call
	return self

## Set horizontal border
func horizontal_border(val: float) -> StyleRef:
	for __style in __styles:
		assert(__style)

		# Set border
		__style.border_width_left = val
		__style.border_width_right = val

	# Chain call
	return self

## Set vertical border
func vertical_border(val: float) -> StyleRef:
	for __style in __styles:
		assert(__style)

		# Set border
		__style.border_width_top = val
		__style.border_width_bottom = val

	# Chain call
	return self

## Set all border
func border(val: float) -> StyleRef:
	for __style in __styles:
		assert(__style)

		# Set border
		__style.border_width_left = val
		__style.border_width_top = val
		__style.border_width_right = val
		__style.border_width_bottom = val

	# Chain call
	return self

## Set top left border radius
func top_left_radius(val: float) -> StyleRef:
	for __style in __styles:
		assert(__style and __style is StyleBoxFlat)

		# Set margin
		__style.corner_radius_top_left = val

	# Chain call
	return self

## Set top right border radius
func top_right_radius(val: float) -> StyleRef:
	for __style in __styles:
		assert(__style and __style is StyleBoxFlat)

		# Set margin
		__style.corner_radius_top_right = val

	# Chain call
	return self

## Set bottom left border radius
func bottom_left_radius(val: float) -> StyleRef:
	for __style in __styles:
		assert(__style and __style is StyleBoxFlat)

		# Set margin
		__style.corner_radius_bottom_left = val

	# Chain call
	return self

## Set bottom right border radius
func bottom_right_radius(val: float) -> StyleRef:
	for __style in __styles:
		assert(__style and __style is StyleBoxFlat)

		# Set margin
		__style.corner_radius_bottom_right = val

	# Chain call
	return self

## Set all border radius
func radius(val: float) -> StyleRef:
	for __style in __styles:
		assert(__style and __style is StyleBoxFlat)

		# Set margin
		__style.corner_radius_top_left = val
		__style.corner_radius_top_right = val
		__style.corner_radius_bottom_left = val
		__style.corner_radius_bottom_right = val

	# Chain call
	return self


