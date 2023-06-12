extends RefCounted
class_name UI

## The root UI
var __root: UI

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

## Called to initialize
func _init(parent: Node, root: UI = null) -> void:
	# Is root
	if root == null:
		assert(parent.has_method("ui_process"), "target node must have a 'ui_process' function")
		# Connect frame process
		parent.get_tree().process_frame.connect(__frame_update)

	__root = root if root else self
	__parent = parent
	__repaint = true

## Called to add a node to self
func add(t: Object, key = null) -> UIRef:
	# Key must be null or string
	assert(key == null or key is String, "'key' must be null or a string")
	assert(key == null or not key.begins_with("__gudui_id:"), "'key' can't start with reserved string '__gudui:'")

	# Get the cached references from type
	var children = __children.get(t)
	if children == null:
		children = {"idx": 0, "nodes": {}}
		__children[t] = children
	
	# Get the key
	var index: String = "__gudui_id:%d" % (children.idx + 1) if key == null else key

	# Get the cached reference
	var ref: UIRef = children.nodes.get(index)

	# Create the cached reference
	if ref == null:
		ref = UIRef.new()
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

## Return style reference
func style(style_callable: Callable) -> UI:
	# Create new style ref
	if not __style:
		__style = StyleRef.new(__parent, false)

	# Call callable
	style_callable.call(__style)

	# Chain call
	return self

## Queues update in root
func queue_update() -> UI:
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

## Internal UI update
func __update(update_callable: Callable) -> void:
	# Clears self
	for t in __children.values():
		# For each node
		for n in t.nodes.values():
			# Mark it to deletion (if inside tree)
			if n.__inside: n.__deletion = true
		
		# Reset index
		t.idx = 0
	
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

## Internal UI idle update
func __idle_update(delta: float) -> void:
	# Update children
	for t in __children.values():
		for c in t.nodes.values():
			c.__idle_update(delta)

#region Common nodes functions

## Adds a label with text 
func label(text: String) -> UIRef:
	# Add label with text property
	return add(Label).prop("text", text)

## Adds a button with text
func button(text: String) -> UIRef:
	return add(Button).prop("text", text)

## Adds a horizontal scroll
func horizontal_scroll(add_children_callable: Callable) -> UIRef:
	return add(ScrollContainer).show(func (ui):
		ui.add(HBoxContainer).expand_fill().show(add_children_callable)
	)

## Adds a vertical scroll
func vertical_scroll(add_children_callable: Callable) -> UIRef:
	return add(ScrollContainer).show(func (ui):
		ui.add(VBoxContainer).expand_fill().show(add_children_callable)
	)

#endregion

