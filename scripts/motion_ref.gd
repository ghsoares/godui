extends RefCounted
class_name MotionRef

## The target node reference
var __node: Node

## Current time
var __time: float

## The target node property
var __key_prop: String

## Current key frame time
var __key_time: float

## Current key frame duration
var __key_duration: float

## Is parallel?
var __key_parallel: bool

## Animation should loop?
var __loop: bool = false

## The keyframes of the animation (each key is a property)
var __keyframes: Dictionary = {}

## Called to initialize
func _init(node: Node) -> void:
	__node = node
	__clear()

## Get property track
func __get_track(name: String) -> Array:
	var track = __keyframes.get(name)
	if track == null:
		assert(__node.get(name) != null)
		track = [{
			"time": __key_time,
			"duration": 0.0,
			"value": __node.get(name),
			"ease": 0.0
		}]
		__keyframes[name] = track
	return track

## Clear the animation
func __clear() -> void:
	# Clear state
	__key_prop = ""
	__key_time = 0.0
	__key_duration = 0.0
	__key_parallel = false
	__loop = false
	__keyframes.clear()

## Called to animate based on time
func __animate() -> void:
	# Frames is empty
	if __keyframes.is_empty(): return
	
	# Should loop
	if __loop:
		__time = fmod(__time, __key_duration) if __key_duration > 0.0 else 0.0

	# For each property track
	for prop in __keyframes:
		# Get track
		var track: Array = __keyframes[prop]

		# Is empty
		if track.is_empty(): continue

		# Get keyframes
		var idx0 = 0
		var idx1 = 0
		for i in range(track.size() - 1, -1, -1):
			if __time >= track[i].time:
				idx0 = max(i - 1, 0)
				idx1 = i
				break

		# Get keyframes info
		var key0: Dictionary = track[idx0]
		var key1: Dictionary = track[idx1]
		
		# Get interpolation factor
		var t: float = (__time - key1.time)
		if key1.duration == 0.0:
			t = 0.0 if __time < key1.time else 1.0
		else:
			t = clamp((__time - key1.time) / key1.duration, 0.0, 1.0)
		t = ease(t, key1.ease)
		
		# Get start and end value
		var val0 = key0.value
		var val1 = key1.value

		# Set interpolated
		__node.set(prop, val0 + (val1 - val0) * t)

## Called to reset the animation
func reset() -> void:
	__time = 0.0

## Called to set loopable
func loop(enabled: bool) -> MotionRef:
	__loop = enabled

	# Chain calls
	return self

## Called to animate multiple properties parallely
func parallel(motion_callable: Callable) -> MotionRef:
	# Store state
	var key_parallel: bool = __key_parallel
	var key_time: float = __key_time
	var key_duration: float = __key_duration

	# Set self parallel to true
	__key_parallel = true

	# Set duration
	__key_duration = 0.0

	# Call callable
	motion_callable.call(self)

	# Seek time
	if not key_parallel:
		__key_time = key_time + __key_duration
		__key_duration = key_duration + __key_duration
	else:
		__key_time = key_time
		__key_duration = max(key_duration, __key_duration)
	
	# Set parallel to previous state
	__key_parallel = key_parallel

	# Chain calls
	return self

## Called to animate a property
func prop(name: String, motion_callable: Callable) -> MotionRef:
	# Store state
	var key_parallel: bool = __key_parallel
	var key_time: float = __key_time
	var key_duration: float = __key_duration

	# Set prop and set parallel to false
	self.__key_prop = name
	self.__key_parallel = false
	self.__key_duration = 0.0

	# Call callable
	motion_callable.call(self)

	# Seek time
	if not key_parallel:
		__key_time = key_time + __key_duration
		__key_duration = key_duration + __key_duration
	else:
		__key_time = key_time
		__key_duration = max(key_duration, __key_duration)
	
	# Set parallel to previous state
	__key_parallel = key_parallel

	# Chain call
	return self

## Add a single transition keyframe
func keyframe(val, duration: float, ease: float) -> MotionRef:
	# Invalid duration
	assert(duration >= 0.0)

	# Get track
	var track: Array = __get_track(__key_prop)

	# Append a single keyframe
	track.append({
		"time": __key_time,
		"duration": duration,
		"value": val,
		"ease": ease
	})

	# Seek time
	if not __key_parallel:
		__key_time += duration
		__key_duration += duration
	else:
		__key_duration = max(__key_duration, duration)

	# Chain call
	return self

## Simply advance time without keyframes
func wait(duration: float) -> MotionRef:
	assert(not __key_parallel, "'wait' doesn't work for parallel motion")
	assert(duration >= 0.0)

	# Advance time and duration
	__key_time += duration
	__key_duration += duration

	# Chain call
	return self

## Repeat a given motion n times
func repeat(times: int, motion_callable: Callable) -> MotionRef:
	# Store state
	var key_parallel: bool = __key_parallel
	var key_time: float = __key_time
	var key_duration: float = __key_duration

	# Set prop and set parallel to false
	self.__key_parallel = false
	self.__key_duration = 0.0

	# Call callable n times
	for i in range(times):
		motion_callable.call(self, i)

	# Seek time
	if not key_parallel:
		__key_time = key_time + __key_duration
		__key_duration = key_duration + __key_duration
	else:
		__key_time = key_time
		__key_duration = max(key_duration, __key_duration)
	
	# Set parallel to previous state
	__key_parallel = key_parallel

	# Chain call
	return self

## Called to set value at time
func frame(val) -> MotionRef:
	return self.keyframe(val, 0.0, 0.0)

## Called to ease to value
func ease(val, duration: float, curve: float) -> MotionRef:
	return self.keyframe(val, duration, curve)

## Called to ease linearly to value
func ease_linear(val, duration: float) -> MotionRef:
	return self.ease(val, duration, 1.0)

## Called to ease in to value
func ease_in(val, duration: float, strength: float = 3.0) -> MotionRef:
	return self.ease(val, duration, strength)

## Called to ease out to value
func ease_out(val, duration: float, strength: float = 3.0) -> MotionRef:
	return self.ease(val, duration, 1.0 / strength)

## Called to ease in-out to value
func ease_in_out(val, duration: float, strength: float = 3.0) -> MotionRef:
	return self.ease(val, duration, -strength)

## Called to ease out-in to value
func ease_out_in(val, duration: float, strength: float = 3.0) -> MotionRef:
	return self.ease(val, duration, -(1.0 / strength))

