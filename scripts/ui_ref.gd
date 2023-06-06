extends RefCounted
class_name UIRef

## The unique index of this reference
var __index: String

## The node reference
var __node: Node

## Is node inside tree?
var __inside: bool

## The child UI
var __children: UI

## Set a single property
func prop(name: String, val) -> UIRef:
	# Set in the node
	__node.set(name, val)

	# Chain call
	return self

## Build the child UI
func children(add_children_callable: Callable) -> UIRef:
	# Creates a new UI
	if __children == null:
		__children = UI.new()
	
	# Clears it
	__children.clear()

	# Set the parent
	__children.__parent = __node

	# Call the callable
	add_children_callable.call(__children)

	return self

