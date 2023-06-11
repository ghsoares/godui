extends RefCounted
class_name MotionRef

## The node reference
var __node: Node

## Current property
var __prop: String

## Current time
var __time: float

## Current track time
var __track_time: float

## Current track next time
var __track_next_time: float

## Called to initialize
func _init(node: Node) -> void:
	__node = node

## Animate property
func prop(name: String, motion_callable: Callable) -> MotionRef:
	# Previous property
	var p: String = __prop

	# Set property
	__prop = name

	# Call callable
	motion_callable.call(self)

	# Set property back
	__prop = p

	return self

## Linearly interpolate to 

