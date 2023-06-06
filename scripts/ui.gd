extends RefCounted
class_name UI

## The parent node
var __parent: Node

## The cached children (type -> children)
var __children: Dictionary

## Called to add a node to self
func add(t: Object, key = null) -> UIRef:
	# Key must be null or string
	assert(key == null or key is String, "'key' must be null or a string")
	assert(not key.begins_with("__gudui_id:"), "'key' can't start with reserved string '__gudui:'")

	# Get the cached references from type
	var children = __children.get(t)
	if children == null:
		children = {"idx": 0, "nodes": {}}
		__children[t] = children
	
	# Get the key
	var index: String = "__gudui_id:%i" % (children.idx + 1) if key == null else key

	# Get the cached reference
	var ref: UIRef = children.nodes.get(index)

	# Create the cached reference
	if ref == null:
		ref = UIRef.new()
		ref.index = index
		ref.node = t.new()
		ref.node.name = "%s:%s" % [str(t), children.idx + 1]
		children.nodes[index] = ref

	# Add to tree
	__parent.add_child(ref.node)

	# Mark as inside tree
	ref.__inside = true

	# Increment the index
	children.idx += 1

	# Return the cached reference
	return ref

## Called to clear self
func clear() -> UI:
	# For each children type
	for t in __children:
		# For each node
		for n in t.nodes:
			# Only remove it from parent if inside tree
			if n.__inside:
				__parent.remove_child(n.node)
				n.__inside = false
		# Reset index
		t.idx = 0

	# Chain call
	return self

"""
ui.add(PanelContainer).props({
	"anchor_left": 0.0, "anchor_right": 0.0,
	"anchor_top": 0.0, "anchor_bottom": 0.0
}).children(func (ui):
	ui.add(VBoxContainer).children(func (ui):
		ui.add(Label).prop("text", "Current number: %s" % [self.selected_number + 1])
		for i in range(0, 64):
			let btn: UIRef = ui.add(Button, "btn %s" % i).prop("text", "Number %s" % [i + 1])
			if i != self.selected_number:
				label.set_theme_color("font_color", Color(1, 1, 1, 0.5))
			else:
				label.set_theme_color("font_color", Color(1, 1, 1))
			btn.signal("pressed", func (): self.selected_number = i)
	)
)
"""
