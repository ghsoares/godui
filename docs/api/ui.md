<span parse-class-api/>

<span class-reference/>

# UI

**Inherits** [RefCounted] < [Object]

## Description

Base class for user interfaces, wraps a node and provide utility functions to modify it. The majority of the functions return the same UI reference to be able to chain functions.

## Methods

<span class-methods>Method list will show from the [docs website](https://ghsoares.github.io/godui/#/api/ui?id=methods)</span>

## Method descriptions

<h3 class-method>void clear_children()</h3>

Clears all the children and remove the respective nodes from the scene.

<h3 class-method>UI ref() const</h3>

Returns the internal [Node] reference.

<h3 class-method>UI add(Object type, Variant key = null, bool persist = false)</h3>

Adds a new node of type `type` to the UI and returns it.

You can provide a custom `key` to the node, which will be used to reference the respective Node, can be a number, string, etc.

`persist` tells if the Node must be keep in memory after being removed as child from parent.

<h3 class-method>UI show(Callable ui_callable)</h3>

Assigns a update callable `ui_callable` to this UI, each time this particular UI changes, this callable is called, it must have only a single argument of type [UI].

<h3 class-method>UI prop(String name, Variant value)</h3>

Sets a property of the [Node] with name `name` to `value`.

<h3 class-method>UI props(Dictionary properties)</h3>

Sets multiple properties from `properties` with each key being the property's name and the value being the property's value.

<h3 class-method>UI motion(Callable motion_callable)</h3>

Creates a [MotionRef] reference if not exists and call `motion_callable` with it to create animations.

<span class-method="UI event(String signal_name, [Callable] target)"/>

Connects the [Node]'s signal of name `signal_name` to the callable `target`.

<h3 class-method>UI queue_update()</h3>

Requests this UI to update next frame.

<h3 class-method>UI root_queue_update()</h3>

Requests this UI's root to update next frame.

<h3 class-method>UI animate_rect(float speed = 10.0)</h3>

Smoothly animate the [Control]'s rect with linear interpolation with speed `speed`.

<h3 class-method>UI theme_variation(String theme_type)</h3>

Assign the [Control]'s `theme_type_variation` property to `theme_type`

<h3 class-method>UI shrink_begin()</h3>

Sets [Control]'s `size_flags_horizontal` and `size_flags_vertical` to `SIZE_SHRINK_BEGIN`

<h3 class-method>UI fill()</h3>

Sets [Control]'s `size_flags_horizontal` and `size_flags_vertical` to `SIZE_FILL`

<h3 class-method>UI expand()</h3>

Sets [Control]'s `size_flags_horizontal` and `size_flags_vertical` to `SIZE_EXPAND`

<h3 class-method>UI expand_fill()</h3>

Sets [Control]'s `size_flags_horizontal` and `size_flags_vertical` to `SIZE_EXPAND`

<h3 class-method>UI shrink_center()</h3>

Sets [Control]'s `size_flags_horizontal` and `size_flags_vertical` to `SIZE_SHRINK_CENTER`

<h3 class-method>UI shrink_end()</h3>

Sets [Control]'s `size_flags_horizontal` and `size_flags_vertical` to `SIZE_SHRINK_END`

<h3 class-method>UI horizontal_shrink_begin()</h3>

Sets [Control]'s `size_flags_horizontal` to `SIZE_SHRINK_BEGIN`

<h3 class-method>UI horizontal_fill()</h3>

Sets [Control]'s `size_flags_horizontal` to `SIZE_FILL`

<h3 class-method>UI horizontal_expand()</h3>

Sets [Control]'s `size_flags_horizontal` to `SIZE_EXPAND`

<h3 class-method>UI horizontal_expand_fill()</h3>

Sets [Control]'s `size_flags_horizontal` to `SIZE_EXPAND`

<h3 class-method>UI horizontal_shrink_center()</h3>

Sets [Control]'s `size_flags_horizontal` to `SIZE_SHRINK_CENTER`

<h3 class-method>UI horizontal_shrink_end()</h3>

Sets [Control]'s `size_flags_horizontal` to `SIZE_SHRINK_END`

<h3 class-method>UI vertical_shrink_begin()</h3>

Sets [Control]'s `size_flags_horizontal` to `SIZE_SHRINK_BEGIN`

<h3 class-method>UI vertical_fill()</h3>

Sets [Control]'s `size_flags_horizontal` to `SIZE_FILL`

<h3 class-method>UI vertical_expand()</h3>

Sets [Control]'s `size_flags_horizontal` to `SIZE_EXPAND`

<h3 class-method>UI vertical_expand_fill()</h3>

Sets [Control]'s `size_flags_horizontal` to `SIZE_EXPAND`

<h3 class-method>UI vertical_shrink_center()</h3>

Sets [Control]'s `size_flags_horizontal` to `SIZE_SHRINK_CENTER`

<h3 class-method>UI vertical_shrink_end()</h3>

Sets [Control]'s `size_flags_horizontal` to `SIZE_SHRINK_END`

<h3 class-method>UI full_rect()</h3>

Sets [Control]'s `anchor_*` and `offset_*` so that the Control's rect takes the entire parent's rect.

<h3 class-method>UI margin(Variant unit)</h3>

Sets [Control]'s `anchor_*` and `offset_*` so that the Control's rect has a margin relative to parent.
`unit` can be of type [int] or [float] for pixels, or [String] in pixels format: `"64px"` or relative format: `"5%"`

<h3 class-method>UI horizontal_margin(Variant unit)</h3>

Sets [Control]'s `anchor_left`, `anchor_right`, `offset_left` and `offset_right` so that the Control's rect has a margin relative to parent.
`unit` can be of type [int] or [float] for pixels, or [String] in pixels format: `"64px"` or relative format: `"5%"`

<h3 class-method>UI vertical_margin(Variant unit)</h3>

Sets [Control]'s `anchor_top`, `anchor_bottom`, `offset_top` and `offset_bottom` so that the Control's rect has a margin relative to parent.
`unit` can be of type [int] or [float] for pixels, or [String] in pixels format: `"64px"` or relative format: `"5%"`

<h3 class-method>UI left_margin(Variant unit)</h3>

Sets [Control]'s `anchor_left` and `offset_left` so that the Control's rect has a margin relative to parent.
`unit` can be of type [int] or [float] for pixels, or [String] in pixels format: `"64px"` or relative format: `"5%"`

<h3 class-method>UI top_margin(Variant unit)</h3>

Sets [Control]'s `anchor_top` and `offset_top` so that the Control's rect has a margin relative to parent.
`unit` can be of type [int] or [float] for pixels, or [String] in pixels format: `"64px"` or relative format: `"5%"`

<h3 class-method>UI right_margin(Variant unit)</h3>

Sets [Control]'s `anchor_right` and `offset_right` so that the Control's rect has a margin relative to parent.
`unit` can be of type [int] or [float] for pixels, or [String] in pixels format: `"64px"` or relative format: `"5%"`

<h3 class-method>UI bottom_margin(Variant unit)</h3>

Sets [Control]'s `anchor_bottom` and `offset_bottom` so that the Control's rect has a margin relative to parent.
`unit` can be of type [int] or [float] for pixels, or [String] in pixels format: `"64px"` or relative format: `"5%"`

<h3 class-method>UI create(Node node)static</h3>

Creates a new [UI] binding it to `node`. `Node` must maintain reference to the returned UI and must have a function `ui_process(UI ui)`.

