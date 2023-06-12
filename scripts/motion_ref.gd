extends RefCounted
class_name MotionRef

## The node reference
var __node: Node

## Current track
var __track: Dictionary

## Current time
var __time: float

## Animation keyframes
var __keyframes: Dictionary

## Called to initialize
func _init(node: Node) -> void:
	__node = node

## Called to clear the keyframes
func clear() -> void:
	__keyframes.clear()

## Animate property
func prop(name: String, motion_callable: Callable) -> MotionRef:
	# Previous track
	var t: Dictionary = __track

	# Check if already exists
	var track = __keyframes.get(name)
	if track == null:
		track = {
			"time": 0.0,
			"len": 0.0,
			"frames": []
		}
		__keyframes[name] = track
	
	# Set track
	__track = track

	# Call callable
	motion_callable.call(self)

	# Set track back
	__track = t

	return self

## Initialize with initial value
func initial(val) -> MotionRef:
	var time: float = __track.time
	var len: float = __track.len
	var frames: Array = __track.frames

	frames.append({
		"time": time,
		
	})

	return self

