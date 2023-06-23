[ParseClassAPI]

[ClassReference]

# MotionRef

**Inherits** [RefCounted] < [Object]

## Description

Motion system provides utility to easily create [UI] animations.

## Methods

[Methods]

## Method descriptions

[Method](void reset())

Resets current Motion's time, use this to change between animations.

[Method](MotionRef loop(bool enabled))

Sets if the Motion should loop when animation ends.

[Method](MotionRef scope(Callable motion_callable))

[EmptyMethod]

[Method](MotionRef parallel(Callable motion_callable))

Animate multiple tracks parallely, useful to animate multiple properties at the same time.

[Method](MotionRef chain(Callable motion_callable))

Inverse of [parallel](#parallel), animates tracks one after the other, useful to animate keyframes inside parallel tracks.

[Method](MotionRef scale(Callable motion_callable))

Set's the duration scales of the animated keys, it's inversely proportional to the key's velocity, the lower the scale, the faster the key will be animated.

[Method](MotionRef prop(String name, bool indexed = false))

Animates a single property, call this before animating keys, set `indexed` to true if you want to work with indexed properties, for example `color:a`.

[Method](MotionRef callback(Callable callback_callable))

Inserts a single callback keyframe, called after previous keys were animated.

[Method](MotionRef wait(float duration))

Insert a empty animation keyframe that lasts `duration` seconds until next keyframe.

[Method](MotionRef repeat(int times, Callable motion_callable))

Repeats keyframes multiple times, it passes the repetition index to `motion_callable`.

[Method](Variant current())

Gets current property track's value, must call [prop](#prop) first.

[Method](Variant relative(Variant delta))

Gets a value relative to the current property track's value by `delta`, must call [prop](#prop) first.

[Method](Variant frame(Variant value))

Inserts a single keyframe without duration for the current property track.

[Method](Variant from_current())

Inserts a single keyframe from the current property track's value or the node's property value. Same as calling `frame(current())`

[Method](Variant constant(Variant to_value, float duration))

Animates using constant transition to target value `to_value` during `duration` seconds.

[Method](Variant linear(Variant to_value, float duration))

Animates using linear transition to target value `to_value` during `duration` seconds.

[Method](Variant ease_in(Variant to_value, float duration, float strength = 3.0))

Animates using in-easing transition to target value `to_value` during `duration` seconds. Set `strength` to the strength of the easing.

[Method](Variant ease_out(Variant to_value, float duration, float strength = 3.0))

Animates using out-easing transition to target value `to_value` during `duration` seconds. Set `strength` to the strength of the easing.

[Method](Variant ease_in_out(Variant to_value, float duration, float strength = 3.0))

Animates using in-out-easing transition to target value `to_value` during `duration` seconds. Set `strength` to the strength of the easing.

[Method](Variant ease_out_in(Variant to_value, float duration, float strength = 3.0))

Animates using out-in-easing transition to target value `to_value` during `duration` seconds. Set `strength` to the strength of the easing.

[Method](Variant elastic_in(Variant to_value, float duration, float strength = 3.0))

Animates using in-elastic transition to target value `to_value` during `duration` seconds. Set `strength` to the strength of the elastic animation.

[Method](Variant elastic_out(Variant to_value, float duration, float strength = 3.0))

Animates using out-elastic transition to target value `to_value` during `duration` seconds. Set `strength` to the strength of the elastic animation.

[Method](Variant elastic_in_out(Variant to_value, float duration, float strength = 3.0))

Animates using in-out-elastic transition to target value `to_value` during `duration` seconds. Set `strength` to the strength of the elastic animation.

[Method](Variant elastic_out_in(Variant to_value, float duration, float strength = 3.0))

Animates using out-in-elastic transition to target value `to_value` during `duration` seconds. Set `strength` to the strength of the elastic animation.

[Method](Variant pulse(Variant to_value, float duration, float strength = 3.0))

Pulses to target value `to_value` then goes back to the initial value before this keyframe during `duration` seconds. Set `strength` to the strength of the pulse animation.

[Method](Variant shake(Variant to_value, float duration, float strength = 3.0))

Shakes to target value `to_value` then goes back to the initial value before this keyframe during `duration` seconds. Set `strength` to the frequency of the shake animation.
