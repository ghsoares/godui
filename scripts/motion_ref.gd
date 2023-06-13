extends RefCounted
class_name MotionRef

## Class used to dynamically animate properties, the animation keyframes are updated alongside with the UI's update, and are processed every frame

## The types of tracks
enum TrackType {
	Property,
	Callback
}

## The target node reference
var __node: Node

## Previous time
var __prev_time: float

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

## Motion should loop?
var __loop: bool = false

## The tracks of the motion
var __tracks: Dictionary = {}

## Current track of the motion
var __track: Array = []

## Called to initialize
func _init(node: Node) -> void:
	__node = node
	__clear()

## Get a property track
func __get_property_track(name: String) -> Array:
	# Get tracks of type property
	var property_track = __tracks.get(TrackType.Property)
	if property_track == null:
		property_track = {}
		__tracks[TrackType.Property] = property_track
	
	# Get track for property
	var track = property_track.get(name)
	if track == null:
		assert(__node.get(name) != null)
		track = [{
			"time": __key_time,
			"duration": 0.0,
			"value": __node.get(name),
			"ease": 0.0
		}]
		property_track[name] = track
	return track

## Get the callback track
func __get_callback_track() -> Array:
	# Get tracks of type callback
	var track = __tracks.get(TrackType.Callback)
	if track == null:
		track = []
		__tracks[TrackType.Callback] = track
	
	# Return the track
	return track

## Clear the motion
func __clear() -> void:
	# Clear state
	__key_prop = ""
	__key_time = 0.0
	__key_duration = 0.0
	__key_parallel = false
	__loop = false
	__tracks.clear()

## Called to animate based on time
func __animate() -> void:
	# Frames is empty
	if __tracks.is_empty(): return
	
	# Loop animation
	if __loop:
		__update_keyframes()
		if __time > __key_duration:
			__prev_time -= __key_duration
			__time -= __key_duration
			__update_keyframes()
	else:
		__update_keyframes()

	# Set previous time
	__prev_time = __time

## Called to update keyframes based on a time
func __update_keyframes() -> void:
	# Get property track
	var prop_track = __tracks.get(TrackType.Property)
	if prop_track != null:
		# For each property track
		for prop in prop_track:
			# Get track
			var track: Array = prop_track[prop]

			# Get track size
			var track_size: int = track.size()

			# Is empty
			if track.is_empty(): continue
			
			# Get keyframe index
			var idx: int = 0

			# Get keyframes
			for i in range(track_size - 1, -1, -1):
				# Get keyframe info
				var key: Dictionary = track[i]
				
				# Get start and end time
				var t0: float = key.time
				var t1: float = key.time + key.duration

				# Check if overlaps
				if __time >= t0:
					idx = i
					break
			
			# Get previous keyframe index
			var prev_idx: int = max(idx - 1, 0)

			# Get keyframes info
			var key0: Dictionary = track[prev_idx]
			var key1: Dictionary = track[idx]
			
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

	# Get callback track
	var call_track = __tracks.get(TrackType.Callback)
	if call_track != null:
		# Get track
		var track: Array = call_track

		# Get track size
		var track_size: int = track.size()

		# Is empty
		if not track.is_empty():
			# Get keyframes
			for i in range(track_size - 1, -1, -1):
				# Get keyframe info
				var key: Dictionary = track[i]
				
				# Get callback time
				var t: float = key.time

				# Check if overlaps
				if __prev_time <= t and __time >= t:
					# Call it
					key.callable.call()

## Called to reset the motion
func reset() -> void:
	__time = 0.0
	__prev_time = 0.0

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

## Inverse of parallel, used to chain multiple properties one after another
func chain(motion_callable: Callable) -> MotionRef:
	# Store state
	var key_parallel: bool = __key_parallel
	var key_time: float = __key_time
	var key_duration: float = __key_duration

	# Set self parallel to false
	__key_parallel = false

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

## Called to animate a property track
func prop(name: String, motion_callable: Callable) -> MotionRef:
	# Store state
	var key_parallel: bool = __key_parallel
	var key_time: float = __key_time
	var key_duration: float = __key_duration

	# Set prop and set parallel to false
	self.__key_prop = name
	self.__key_parallel = false
	self.__key_duration = 0.0

	# Get property track
	__track = __get_property_track(name)

	# Call callable
	motion_callable.call(self)

	# Set track to empty
	__track = []

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

	# Append a single property keyframe
	__track.append({
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

## Called to call a function
func callback(callable: Callable) -> MotionRef:
	# Get callback track
	__track = __get_callback_track()

	# Append a single callback keyframe
	__track.append({
		"time": __key_time,
		"callable": callable
	})

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
func ease_in(val, duration: float, strength: float = 2.0) -> MotionRef:
	return self.ease(val, duration, strength)

## Called to ease out to value
func ease_out(val, duration: float, strength: float = 2.0) -> MotionRef:
	return self.ease(val, duration, 1.0 / strength)

## Called to ease in-out to value
func ease_in_out(val, duration: float, strength: float = 2.0) -> MotionRef:
	return self.ease(val, duration, -strength)

## Called to ease out-in to value
func ease_out_in(val, duration: float, strength: float = 2.0) -> MotionRef:
	return self.ease(val, duration, -(1.0 / strength))

