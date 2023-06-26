<span parse-class-api/>

# MotionRef

**Inherits** <span class-ref>RefCounted</span> < <span class-ref>Object</span>

## Description

Motion system provides utility to easily create <span class-ref>UI</span> animations.

## Methods

<span class-methods>Method list will show from the [docs website](https://ghsoares.github.io/godui/#/api/motion_ref?id=methods)</span>

## Method descriptions

<h3 class-method>void reset()</h3>

Resets current Motion's time, use this to change between animations.

<h3 class-method>MotionRef loop(bool enabled)</h3>

Sets if the Motion should loop when animation ends.

<h3 class-method>MotionRef scope(Callable motion_callable)</h3>

<span class-empty-method/>

<h3 class-method>MotionRef parallel(Callable motion_callable)</h3>

Animate multiple tracks parallely, useful to animate multiple properties at the same time.

<h3 class-method>MotionRef chain(Callable motion_callable)</h3>

Inverse of <span class-member-ref>parallel</span>, animates tracks one after the other, useful to animate keyframes inside parallel tracks.

<h3 class-method>MotionRef scale(Callable motion_callable)</h3>

Set's the duration scales of the animated keys, it's inversely proportional to the key's velocity, the lower the scale, the faster the key will be animated.

<h3 class-method>MotionRef prop(String name, bool indexed = false)</h3>

Animates a single property, call this before animating keys, set `indexed` to true if you want to work with indexed properties, for example `color:a`.

<h3 class-method>MotionRef callback(Callable callback_callable)</h3>

Inserts a single callback keyframe, called after previous keys were animated.

<h3 class-method>MotionRef wait(float duration)</h3>

Insert a empty animation keyframe that lasts `duration` seconds until next keyframe.

<h3 class-method>MotionRef repeat(int times, Callable motion_callable)</h3>

Repeats keyframes multiple times, it passes the repetition index to `motion_callable`.

<h3 class-method>Variant current()</h3>

Gets current property track's value, must call <span class-member-ref>prop</span> first.

<h3 class-method>Variant relative(Variant delta)</h3>

Gets a value relative to the current property track's value by `delta`, must call <span class-member-ref>prop</span> first.

<h3 class-method>Variant frame(Variant value)</h3>

Inserts a single keyframe without duration for the current property track.

<h3 class-method>Variant from_current()</h3>

Inserts a single keyframe from the current property track's value or the node's property value. Same as calling `frame(current())`

<h3 class-method>Variant constant(Variant to_value, float duration)</h3>

Animates using constant transition to target value `to_value` during `duration` seconds.

<h3 class-method>Variant linear(Variant to_value, float duration)</h3>

Animates using linear transition to target value `to_value` during `duration` seconds.

<h3 class-method>Variant ease_in(Variant to_value, float duration, float strength = 3.0)</h3>

Animates using in-easing transition to target value `to_value` during `duration` seconds. Set `strength` to the strength of the easing.

<h3 class-method>Variant ease_out(Variant to_value, float duration, float strength = 3.0)</h3>

Animates using out-easing transition to target value `to_value` during `duration` seconds. Set `strength` to the strength of the easing.

<h3 class-method>Variant ease_in_out(Variant to_value, float duration, float strength = 3.0)</h3>

Animates using in-out-easing transition to target value `to_value` during `duration` seconds. Set `strength` to the strength of the easing.

<h3 class-method>Variant ease_out_in(Variant to_value, float duration, float strength = 3.0)</h3>

Animates using out-in-easing transition to target value `to_value` during `duration` seconds. Set `strength` to the strength of the easing.

<h3 class-method>Variant elastic_in(Variant to_value, float duration, float strength = 3.0)</h3>

Animates using in-elastic transition to target value `to_value` during `duration` seconds. Set `strength` to the strength of the elastic animation.

<h3 class-method>Variant elastic_out(Variant to_value, float duration, float strength = 3.0)</h3>

Animates using out-elastic transition to target value `to_value` during `duration` seconds. Set `strength` to the strength of the elastic animation.

<h3 class-method>Variant elastic_in_out(Variant to_value, float duration, float strength = 3.0)</h3>

Animates using in-out-elastic transition to target value `to_value` during `duration` seconds. Set `strength` to the strength of the elastic animation.

<h3 class-method>Variant elastic_out_in(Variant to_value, float duration, float strength = 3.0)</h3>

Animates using out-in-elastic transition to target value `to_value` during `duration` seconds. Set `strength` to the strength of the elastic animation.

<h3 class-method>Variant pulse(Variant to_value, float duration, float strength = 3.0)</h3>

Pulses to target value `to_value` then goes back to the initial value before this keyframe during `duration` seconds. Set `strength` to the strength of the pulse animation.

<h3 class-method>Variant shake(Variant to_value, float duration, float strength = 3.0)</h3>

Shakes to target value `to_value` then goes back to the initial value before this keyframe during `duration` seconds. Set `strength` to the frequency of the shake animation.
