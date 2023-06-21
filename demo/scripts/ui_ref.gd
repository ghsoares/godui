extends RefCounted
class_name _UIRef

## A simple NULL constant
const __NULL: Dictionary = {}

## The unique index of this reference
var __index: String

## The _UI node reference
var __ui: _UI

## The node reference
var __node: Node

## The node signals
var __signals: Dictionary

## Queued to deletion?
var __deletion: bool

## Is node inside tree?
var __inside: bool

## The child _UI
var __children: _UI

## Current style reference
var __style: StyleRef

## Current motion reference
var __motion: _MotionRef

## The unit regex
var __unit_regex: RegEx

## Called to initialize
func _init() -> void:
	# Create the unit regex
	__unit_regex = RegEx.new()
	__unit_regex.compile("^(\\d+(?:\\.\\d+)?) *(px|%)?$")

## Set a single property
func prop(name: StringName, val) -> _UIRef:
	# Set in the node
	if __node.get(name) != val:
		__node.set(name, val)

	# Chain call
	return self

## Set multiple properties
func props(props: Dictionary) -> _UIRef:
	# For each key -> value pair, call prop
	for k in props.keys():
		prop(k, props[k])
	
	# Chain call
	return self

## Connect to a signal
func sig(name: StringName, target: Callable) -> _UIRef:
	# Get signal info
	var sig_info = __signals.get(name)
	if sig_info == null:
		sig_info = {"target": null, "disconnect": true}
		__signals[name] = sig_info

	# Must not have a connection yet
	assert(sig_info.disconnect, "Already connected")

	# Connect
	if sig_info.target == null:
		__node.connect(name, target)
	
		# Set target
		sig_info.target = target
	
	# Reconnect
	if sig_info.hash() != target.hash():
		__node.disconnect(name, sig_info.target)
		__node.connect(name, target)

		# Set target
		sig_info.target = target

	# Set disconnect to false
	sig_info.disconnect = false

	# Chain calls
	return self

## Return style reference
func style(style_callable: Callable) -> _UIRef:
	# Create new style ref
	if not __style:
		__style = StyleRef.new(__node, true)

	# Call callable
	style_callable.call(__style)

	return self

## Return animation reference
func motion(motion_callable: Callable) -> _UIRef:
	# Create new motion ref
	if not __motion:
		__motion = _MotionRef.new(__node)

	# Call callable
	motion_callable.call(__motion)

	return self

## Set theme type variation
func theme_variation(name: StringName) -> _UIRef:
	assert(__node is Control)

	# Set type variation
	__node.theme_type_variation = name

	return self

## Build the child _UI
func show(update_child_ui_callable: Callable) -> _UIRef:
	# Creates a new _UI
	if __children == null:
		__children = _UI.new(__node, __ui.__root)

	# Update the _UI
	__children.__update(update_child_ui_callable)

	return self

## Get the node reference
func ref() -> Node: 
	return __node

## Clear self
func __clear() -> void:
	# Mark as not deletion
	__deletion = false
	
	# For each signal
	for sig in __signals.values():
		# Set disconnect to true
		if sig.target != null:
			sig.disconnect = true
	
	# Clear motion
	if __motion:
		__motion.__clear()

## Internal _UI idle update
func __idle_update(delta: float) -> void:
	# Must be inside tree
	if __inside:
		# Has an animation
		if __motion:
			__motion.__time += delta
			__motion.__animate()

	# Update children
	if __children:
		__children.__idle_update(delta)

## Update self
func __post_update() -> void:
	# For each signal
	for sig in __signals.keys():
		# Get signal info
		var sig_info: Dictionary = __signals[sig]

		# Must disconnect
		if sig_info.disconnect:
			__node.disconnect(sig, sig_info.target)
			sig_info.target = null
			sig_info.disconnect = false

## Remove self
func __remove() -> void:
	# Remove from parent node
	__ui.__parent.remove_child(__node)
	
	# For each signal
	for sig in __signals.keys():
		# Get signal info
		var sig_info: Dictionary = __signals[sig]

		# Has a target
		if sig_info.target:
			__node.disconnect(sig, sig_info.target)
	
	# Reset motion
	if __motion:
		__motion.reset()

	# Clear signals
	__signals.clear()

	# Mark inside as false
	__inside = false

	# Mark deletion as false
	__deletion = false

#region Sizing functions

## Set size flags in a axis
func axis_size_flags(axis: int, flags: int) -> _UIRef:
	# Assert that is a control
	assert(__node is Control)

	# Assert valid axis
	assert(axis == 0 or axis == 1, "'axis' parameter must be 0 (horizontal) or 1 (vertical)")

	# Set flag
	if axis == 0:
		__node.size_flags_horizontal = flags
	else:
		__node.size_flags_vertical = flags

	return self

## Expand fill in both axes
func expand_fill() -> _UIRef:
	# Set flags
	axis_size_flags(0, Control.SIZE_EXPAND_FILL)
	axis_size_flags(1, Control.SIZE_EXPAND_FILL)

	# Chain calls
	return self

## Shrink center in both axes
func shrink_center() -> _UIRef:
	# Set flags
	axis_size_flags(0, Control.SIZE_SHRINK_CENTER)
	axis_size_flags(1, Control.SIZE_SHRINK_CENTER)

	# Chain calls
	return self

## Shrink begin the control horizontally
func horizontal_shrink_begin() -> _UIRef:
	# Set flags
	axis_size_flags(0, Control.SIZE_SHRINK_BEGIN)

	# Chain calls
	return self

## Expand the control horizontally
func horizontal_expand() -> _UIRef:
	# Set flags
	axis_size_flags(0, Control.SIZE_EXPAND)

	# Chain calls
	return self

## Expand fill the control horizontally
func horizontal_expand_fill() -> _UIRef:
	# Set flags
	axis_size_flags(0, Control.SIZE_EXPAND_FILL)

	# Chain calls
	return self

## Shrink center the control horizontally
func horizontal_shrink_center() -> _UIRef:
	# Set flags
	axis_size_flags(0, Control.SIZE_SHRINK_CENTER)

	# Chain calls
	return self

## Shrink begin the control vertically
func vertical_shrink_begin() -> _UIRef:
	# Set flags
	axis_size_flags(1, Control.SIZE_SHRINK_BEGIN)

	# Chain calls
	return self

## Expand the control vertically
func vertical_expand() -> _UIRef:
	# Set flags
	axis_size_flags(1, Control.SIZE_EXPAND)

	# Chain calls
	return self

## Expand fill the control vertically
func vertical_expand_fill() -> _UIRef:
	# Set flags
	axis_size_flags(1, Control.SIZE_EXPAND_FILL)

	# Chain calls
	return self

## Shrink center the control vertically
func vertical_shrink_center() -> _UIRef:
	# Set flags
	axis_size_flags(1, Control.SIZE_SHRINK_CENTER)

	# Chain calls
	return self

#endregion

#region Anchor placement functions

## Utility function to extract anchor and offset from unit
func __extract_anchor_unit(unit: String, invert: bool) -> PackedFloat32Array:
	# Must find a match
	var m: RegExMatch = __unit_regex.search(unit)
	assert(m, "Invalid unit format, must be a valid number suffixed with either 'px', '%' or nothing")

	# Get anchor and offset
	var a: PackedFloat32Array
	a.resize(2)

	# Percentage
	if m.get_string(2) == "%":
		a[0] = float(m.get_string(1)) / 100.0
		a[1] = 0.0
	# Relative
	else:
		a[0] = 0.0
		a[1] = float(m.get_string(1))
	
	# Invert units
	if invert: 
		a[0] = 1.0 - a[0]
		a[1] = -a[1]
	
	# Return the anchor and offset
	return a

## Positionate the left margin of the control based on a unit (String with suffixes % or px or a number in pixels)
func left_margin(unit) -> _UIRef:
	assert(__node is Control)
	assert(unit is String or unit is float or unit is int, "'unit' parameter must be a String, float or int")

	# Is a string
	if unit is String:
		# Set anchor and offset
		var anchor: PackedFloat32Array = __extract_anchor_unit(unit, false)
		__node.anchor_left = anchor[0]
		__node.offset_left = anchor[1]
	# Is a number
	else:
		__node.anchor_left = 0.0
		__node.offset_left = unit
	
	# Chain calls
	return self

## Positionate the top margin of the control based on a unit (String with suffixes % or px or a number in pixels)
func top_margin(unit) -> _UIRef:
	assert(__node is Control)
	assert(unit is String or unit is float or unit is int, "'unit' parameter must be a String, float or int")

	# Is a string
	if unit is String:
		# Set anchor and offset
		var anchor: PackedFloat32Array = __extract_anchor_unit(unit, false)
		__node.anchor_top = anchor[0]
		__node.offset_top = anchor[1]
	# Is a number
	else:
		__node.anchor_top = 0.0
		__node.offset_top = unit
	
	# Chain calls
	return self

## Positionate the right margin of the control based on a unit (String with suffixes % or px or a number in pixels)
func right_margin(unit) -> _UIRef:
	assert(__node is Control)
	assert(unit is String or unit is float or unit is int, "'unit' parameter must be a String, float or int")

	# Is a string
	if unit is String:
		# Set anchor and offset
		var anchor: PackedFloat32Array = __extract_anchor_unit(unit, true)
		__node.anchor_right = anchor[0]
		__node.offset_right = anchor[1]
	# Is a number
	else:
		__node.anchor_right = 1.0
		__node.offset_right = -unit
	
	# Chain calls
	return self

## Positionate the bottom margin of the control based on a unit (String with suffixes % or px or a number in pixels)
func bottom_margin(unit) -> _UIRef:
	assert(__node is Control)
	assert(unit is String or unit is float or unit is int, "'unit' parameter must be a String, float or int")

	# Is a string
	if unit is String:
		# Set anchor and offset
		var anchor: PackedFloat32Array = __extract_anchor_unit(unit, true)
		__node.anchor_bottom = anchor[0]
		__node.offset_bottom = anchor[1]
	# Is a number
	else:
		__node.anchor_bottom = 1.0
		__node.offset_bottom = -unit
	
	# Chain calls
	return self

## Positionate the horizontal anchors based on a unit margin (String with suffixes % or px or a number in pixels)
func horizontal_margin(unit) -> _UIRef:
	assert(__node is Control)
	assert(unit is String or unit is float or unit is int, "'unit' parameter must be a String, float or int")

	# Is a string
	if unit is String:
		# Set anchor and offset
		var anchor: PackedFloat32Array = __extract_anchor_unit(unit, false)
		__node.anchor_left = anchor[0]
		__node.anchor_right = 1.0 - anchor[0]
		__node.offset_left = anchor[1]
		__node.offset_right = -anchor[1]
	# Is a number
	else:
		__node.anchor_left = 0.0
		__node.anchor_right = 1.0
		__node.offset_left = unit
		__node.offset_right = -unit
	
	# Chain calls
	return self

## Positionate the vertical anchors based on a unit margin (String with suffixes % or px or a number in pixels)
func vertical_margin(unit) -> _UIRef:
	assert(__node is Control)
	assert(unit is String or unit is float or unit is int, "'unit' parameter must be a String, float or int")

	# Is a string
	if unit is String:
		# Set anchor and offset
		var anchor: PackedFloat32Array = __extract_anchor_unit(unit, false)
		__node.anchor_top = anchor[0]
		__node.anchor_bottom = 1.0 - anchor[0]
		__node.offset_top = anchor[1]
		__node.offset_bottom = -anchor[1]
	# Is a number
	else:
		__node.anchor_top = 0.0
		__node.anchor_bottom = 1.0
		__node.offset_top = unit
		__node.offset_bottom = -unit
	
	# Chain calls
	return self

## Positionate all the anchors based on a unit margin (String with suffixes % or px or a number in pixels)
func margin(unit) -> _UIRef:
	assert(__node is Control)
	assert(unit is String or unit is float or unit is int, "'unit' parameter must be a String, float or int")

	# Is a string
	if unit is String:
		# Set anchor and offset
		var anchor: PackedFloat32Array = __extract_anchor_unit(unit, false)
		__node.anchor_left = anchor[0]
		__node.anchor_top = anchor[0]
		__node.anchor_right = 1.0 - anchor[0]
		__node.anchor_bottom = 1.0 - anchor[0]
		__node.offset_left = anchor[1]
		__node.offset_top = anchor[1]
		__node.offset_right = -anchor[1]
		__node.offset_bottom = -anchor[1]
	# Is a number
	else:
		__node.anchor_left = 0.0
		__node.anchor_top = 0.0
		__node.anchor_right = 1.0
		__node.anchor_bottom = 1.0
		__node.offset_left = unit
		__node.offset_top = unit
		__node.offset_right = -unit
		__node.offset_bottom = -unit
	
	# Chain calls
	return self

## Positionate all the anchors to fill the parent rect
func full_rect() -> _UIRef:
	assert(__node is Control)

	# Set anchors and offset
	__node.anchor_left = 0.0
	__node.anchor_top = 0.0
	__node.anchor_right = 1.0
	__node.anchor_bottom = 1.0
	__node.offset_left = 0.0
	__node.offset_top = 0.0
	__node.offset_right = 0.0
	__node.offset_bottom = 0.0

	# Chain calls
	return self

#endregion


