[ParseClassAPI]

[ClassReference]

# UI

**Inherits** [RefCounted] < [Object]

## Description

Base class for user interfaces, wraps a node and provide utility functions to modify it. The majority of the functions return the same UI reference to be able to chain functions.

## Methods

[Methods]

## Method descriptions

[Method](void clear_children())

Clears all the children and remove the respective nodes from the scene.

[Method](UI ref() const)

Returns the internal [Node] reference.

[Method](UI add(Object type, Variant key = null, bool persist = false))

Adds a new node of type `type` to the UI and returns it.

You can provide a custom `key` to the node, which will be used to reference the respective Node, can be a number, string, etc.

`persist` tells if the Node must be keep in memory after being removed as child from parent.

[Method](UI show(Callable ui_callable))

Assigns a update callable `ui_callable` to this UI, each time this particular UI changes, this callable is called, it must have only a single argument of type [UI].

[Method](UI prop(String name, Variant value))

Sets a property of the [Node] with name `name` to `value`.

[Method](UI props(Dictionary properties))

Sets multiple properties from `properties` with each key being the property's name and the value being the property's value.

[Method](UI motion(Callable motion_callable))

Creates a [MotionRef] reference if not exists and call `motion_callable` with it to create animations.

[Method](UI event(String signal_name, [Callable] target))

Connects the [Node]'s signal of name `signal_name` to the callable `target`.

[Method](UI queue_update())

Requests this UI to update next frame.

[Method](UI root_queue_update())

Requests this UI's root to update next frame.

[Method](UI animate_rect(float speed = 10.0))

Smoothly animate the [Control]'s rect with linear interpolation with speed `speed`.

[Method](UI theme_variation(String theme_type))

Assign the [Control]'s `theme_type_variation` property to `theme_type`

[Method](UI shrink_begin())

Sets [Control]'s `size_flags_horizontal` and `size_flags_vertical` to `SIZE_SHRINK_BEGIN`

[Method](UI fill())

Sets [Control]'s `size_flags_horizontal` and `size_flags_vertical` to `SIZE_FILL`

[Method](UI expand())

Sets [Control]'s `size_flags_horizontal` and `size_flags_vertical` to `SIZE_EXPAND`

[Method](UI expand_fill())

Sets [Control]'s `size_flags_horizontal` and `size_flags_vertical` to `SIZE_EXPAND`

[Method](UI shrink_center())

Sets [Control]'s `size_flags_horizontal` and `size_flags_vertical` to `SIZE_SHRINK_CENTER`

[Method](UI shrink_end())

Sets [Control]'s `size_flags_horizontal` and `size_flags_vertical` to `SIZE_SHRINK_END`

[Method](UI horizontal_shrink_begin())

Sets [Control]'s `size_flags_horizontal` to `SIZE_SHRINK_BEGIN`

[Method](UI horizontal_fill())

Sets [Control]'s `size_flags_horizontal` to `SIZE_FILL`

[Method](UI horizontal_expand())

Sets [Control]'s `size_flags_horizontal` to `SIZE_EXPAND`

[Method](UI horizontal_expand_fill())

Sets [Control]'s `size_flags_horizontal` to `SIZE_EXPAND`

[Method](UI horizontal_shrink_center())

Sets [Control]'s `size_flags_horizontal` to `SIZE_SHRINK_CENTER`

[Method](UI horizontal_shrink_end())

Sets [Control]'s `size_flags_horizontal` to `SIZE_SHRINK_END`

[Method](UI vertical_shrink_begin())

Sets [Control]'s `size_flags_horizontal` to `SIZE_SHRINK_BEGIN`

[Method](UI vertical_fill())

Sets [Control]'s `size_flags_horizontal` to `SIZE_FILL`

[Method](UI vertical_expand())

Sets [Control]'s `size_flags_horizontal` to `SIZE_EXPAND`

[Method](UI vertical_expand_fill())

Sets [Control]'s `size_flags_horizontal` to `SIZE_EXPAND`

[Method](UI vertical_shrink_center())

Sets [Control]'s `size_flags_horizontal` to `SIZE_SHRINK_CENTER`

[Method](UI vertical_shrink_end())

Sets [Control]'s `size_flags_horizontal` to `SIZE_SHRINK_END`

[Method](UI full_rect())

Sets [Control]'s `anchor_*` and `offset_*` so that the Control's rect takes the entire parent's rect.

[Method](UI margin(Variant unit))

Sets [Control]'s `anchor_*` and `offset_*` so that the Control's rect has a margin relative to parent.
`unit` can be of type [int] or [float] for pixels, or [String] in pixels format: `"64px"` or relative format: `"5%"`

[Method](UI horizontal_margin(Variant unit))

Sets [Control]'s `anchor_left`, `anchor_right`, `offset_left` and `offset_right` so that the Control's rect has a margin relative to parent.
`unit` can be of type [int] or [float] for pixels, or [String] in pixels format: `"64px"` or relative format: `"5%"`

[Method](UI vertical_margin(Variant unit))

Sets [Control]'s `anchor_top`, `anchor_bottom`, `offset_top` and `offset_bottom` so that the Control's rect has a margin relative to parent.
`unit` can be of type [int] or [float] for pixels, or [String] in pixels format: `"64px"` or relative format: `"5%"`

[Method](UI left_margin(Variant unit))

Sets [Control]'s `anchor_left` and `offset_left` so that the Control's rect has a margin relative to parent.
`unit` can be of type [int] or [float] for pixels, or [String] in pixels format: `"64px"` or relative format: `"5%"`

[Method](UI top_margin(Variant unit))

Sets [Control]'s `anchor_top` and `offset_top` so that the Control's rect has a margin relative to parent.
`unit` can be of type [int] or [float] for pixels, or [String] in pixels format: `"64px"` or relative format: `"5%"`

[Method](UI right_margin(Variant unit))

Sets [Control]'s `anchor_right` and `offset_right` so that the Control's rect has a margin relative to parent.
`unit` can be of type [int] or [float] for pixels, or [String] in pixels format: `"64px"` or relative format: `"5%"`

[Method](UI bottom_margin(Variant unit))

Sets [Control]'s `anchor_bottom` and `offset_bottom` so that the Control's rect has a margin relative to parent.
`unit` can be of type [int] or [float] for pixels, or [String] in pixels format: `"64px"` or relative format: `"5%"`

[Method](UI create(Node node) static)

Creates a new [UI] binding it to `node`. `Node` must maintain reference to the returned UI and must have a function `ui_update(UI ui)`.

