extends RefCounted
class_name _UI

## The root _UI
var __root: _UI

## The parent node
var __parent: Node

## Needs to repaint?
var __repaint: bool

## The cached children (type -> children)
var __children: Dictionary

## Current child index
var __child_idx: int

## Current style reference
var __style: StyleRef

## _UI temporary state (state variables that are forgot to the next frame)
var __temp_state: Dictionary

## Called to initialize
func _init(parent: Node, root: _UI = null) -> void:
	# Is root
	if root == null:
		assert(parent.has_method("ui_process"), "target node must have a 'ui_process' function")
		# Connect frame process
		parent.get_tree().process_frame.connect(__frame_update)

	__root = root if root else self
	__parent = parent
	__repaint = true

## Called to add a node to self
func add(t: Object, key = null) -> _UIRef:
	# If key is not null, convert to string
	if key != null:
		key = str(key)
	
	# Check if it doesn't start with reserved key
	assert(key == null or not key.begins_with("__gudui_id:"), "'key' can't start with reserved string '__gudui:'")

	# Get the cached references from type
	var children = __children.get(t)
	if children == null:
		children = {"idx": 0, "nodes": {}}
		__children[t] = children
	
	# Get the key
	var index: String = "__gudui_id:%d" % (children.idx + 1) if key == null else key

	# Get the cached reference
	var ref: _UIRef = children.nodes.get(index)

	# Create the cached reference
	if ref == null:
		ref = _UIRef.new()
		ref.__index = index
		ref.__ui = self
		ref.__node = t.new()
		ref.__node.name = "%s:%d" % [ref.__node.get_class(), children.idx + 1]
		children.nodes[index] = ref

	# Add to tree
	if not ref.__inside:
		# Add as child of parent
		__parent.add_child(ref.__node)

		# Mark as inside tree
		ref.__inside = true

	# Move child
	__parent.move_child(ref.__node, __child_idx)

	# Clear reference
	ref.__clear()

	# Increment the index
	children.idx += 1

	# Increment the child index
	__child_idx += 1

	# Return the cached reference
	return ref

## Called to get a temporary state
func get_temporary_state(name: String, default_value = null):
	if __temp_state.has(name):
		return __temp_state[name].value
	return default_value

## Called to set a temporary state
func set_temporary_state(name: String, value) -> _UI:
	__temp_state[name] = {
		"value": value,
		"remove": false
	}

	return self

## Called to check if an event were triggered
func event(name: String) -> bool:
	return get_temporary_state("__gudui:event[%s]" % name, false)

## Called to mark an named event as triggered and updates the interface
func trigger_event(name: String) -> _UI:
	return set_temporary_state("__gudui:event[%s]" % name, true)

## Return style reference
func style(style_callable: Callable) -> _UI:
	# Create new style ref
	if not __style:
		__style = StyleRef.new(__parent, false)

	# Call callable
	style_callable.call(__style)

	# Chain call
	return self

## Queues update in root
func queue_update() -> _UI:
	__root.__repaint = true

	# Chain calls
	return self

## Function is called by the scene tree to update the interface
func __frame_update() -> void:
	# Can repaint
	if __repaint:
		var now: int = Time.get_ticks_usec()
		__update(__parent.ui_process)
		var elapsed: int = Time.get_ticks_usec() - now
		print("Update time: %.2f ms" % (elapsed / 1000.0))
	
	# Idle update
	__idle_update(__parent.get_process_delta_time())

## Internal _UI update
func __update(update_callable: Callable) -> void:
	# Clears self
	for t in __children.values():
		# For each node
		for n in t.nodes.values():
			# Mark it to deletion (if inside tree)
			if n.__inside: n.__deletion = true
		
		# Reset index
		t.idx = 0
	
	# Mark temporary values	to remove
	for s in __temp_state.values():
		s.remove = true

	# Reset child index
	__child_idx = 0
	
	# Set repaint to false
	__repaint = false

	# Call update callable
	update_callable.call(self)

	# For each child type
	for t in __children.values():
		# For each node
		for n in t.nodes.values():
			# Update it
			n.__post_update()

			# Remove from parent if deleted
			if n.__deletion:
				n.__remove()
	
	# Remove temporary states
	for k in __temp_state.keys():
		if __temp_state[k].remove:
			__temp_state.erase(k)

## Internal _UI idle update
func __idle_update(delta: float) -> void:
	# Update children
	for t in __children.values():
		for c in t.nodes.values():
			c.__idle_update(delta)

#region Common nodes functions

## Adds a label with text 
func label(text: String) -> _UIRef:
	# Add label with text property
	return add(Label).prop("text", text)

## Adds a button with text
func button(text: String) -> _UIRef:
	return add(Button).prop("text", text)

## Adds a horizontal scroll
func horizontal_scroll(add_children_callable: Callable) -> _UIRef:
	return add(ScrollContainer).show(func (ui):
		ui.add(HBoxContainer).expand_fill().show(add_children_callable)
	)

## Adds a vertical scroll
func vertical_scroll(add_children_callable: Callable) -> _UIRef:
	return add(ScrollContainer).show(func (ui):
		ui.add(VBoxContainer).expand_fill().show(add_children_callable)
	)

#endregion

