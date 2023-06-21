<!-- Links to some types -->

[Variant]: https://docs.godotengine.org/en/stable/classes/class_variant.html
[bool]: https://docs.godotengine.org/en/stable/classes/class_bool.html
[float]: https://docs.godotengine.org/en/stable/classes/class_float.html
[Object]: https://docs.godotengine.org/en/stable/classes/class_object.html
[RefCounted]: https://docs.godotengine.org/en/stable/classes/class_refcounted.html
[String]: https://docs.godotengine.org/en/stable/classes/class_string.html
[Callable]: https://docs.godotengine.org/en/stable/classes/class_callable.html
[Dictionary]: https://docs.godotengine.org/en/stable/classes/class_dictionary.html
[Node]: https://docs.godotengine.org/en/stable/classes/class_node.html
[Control]: https://docs.godotengine.org/en/stable/classes/class_control.html
[UI]: api/ui.md

# UI

**Inherits** [RefCounted] < [Object]

## Description

Base class for user interfaces, wraps a node and provide utility functions to modify it. The majority of the functions return the same UI reference to be able to chain functions.

## Methods

| returns | method |
| --- | --- |
| void | [clear_children](#clear_children) () |
| [Node] | [ref](#ref) () const |
| [UI] | [add](#add) ([Object] type, [Variant] key = null, [bool] persist = false) |
| [UI] | [show](#show) ([Callable] ui_callable) |
| [UI] | [prop](#prop) ([String] name, [Variant] value) |
| [UI] | [props](#props) ([Dictionary] properties) |
| [UI] | [motion](#motion) ([Callable] motion_callable) |
| [UI] | [event](#event) ([String] signal_name, [Callable] target) |
| [UI] | [queue_update](#queue_update) () |
| [UI] | [root_queue_update](#root_queue_update) () |
| [UI] | [animate_rect](#animate_rect) ([float] speed = 10.0) |
| [UI] | [theme_variation](#theme_variation) ([String] theme_type) |
| [UI] | [shrink_begin](#shrink_begin) () |
| [UI] | [fill](#fill) () |
| [UI] | [expand](#expand) () |
| [UI] | [expand_fill](#expand_fill) () |
| [UI] | [shrink_center](#shrink_center) () |
| [UI] | [shrink_end](#shrink_end) () |
| [UI] | [horizontal_shrink_begin](#horizontal_shrink_begin) () |
| [UI] | [horizontal_fill](#horizontal_fill) () |
| [UI] | [horizontal_expand](#horizontal_expand) () |
| [UI] | [horizontal_expand_fill](#horizontal_expand_fill) () |
| [UI] | [horizontal_shrink_center](#horizontal_shrink_center) () |
| [UI] | [horizontal_shrink_end](#horizontal_shrink_end) () |
| [UI] | [vertical_shrink_begin](#vertical_shrink_begin) () |
| [UI] | [vertical_fill](#vertical_fill) () |
| [UI] | [vertical_expand](#vertical_expand) () |
| [UI] | [vertical_expand_fill](#vertical_expand_fill) () |
| [UI] | [vertical_shrink_center](#vertical_shrink_center) () |
| [UI] | [vertical_shrink_end](#vertical_shrink_end) () |
| [UI] | [full_rect](#full_rect) () |
| [UI] | [margin](#margin) ([float] unit) |
| [UI] | [horizontal_margin](#horizontal_margin) ([Variant] unit) |
| [UI] | [vertical_margin](#vertical_margin) ([Variant] unit) |
| [UI] | [left_margin](#left_margin) ([Variant] unit) |
| [UI] | [top_margin](#top_margin) ([Variant] unit) |
| [UI] | [right_margin](#right_margin) ([Variant] unit) |
| [UI] | [bottom_margin](#bottom_margin) ([Variant] unit) |
| [UI] | [create](#create) ([Node] node) static |

## Method descriptions

### clear_children
**Signature**: **void clear_children()**

Clears all the children and remove the respective nodes from the scene.

### ref
**Signature**: **[UI] ref () const**

Returns the internal [Node] reference.

### add
**Signature**: **[UI] add ([Object] type, [Variant] key = null, [bool] persist = false)**

Adds a new node of type `type` to the UI and returns it.

You can provide a custom `key` to the node, which will be used to reference the respective Node, can be a number, string, etc.

`persist` tells if the Node must be keep in memory after being removed as child from parent.

### show
**Signature**: **[UI] show ([Callable] ui_callable)**

Assigns a update callable `ui_callable` to this UI, each time this particular UI changes, this callable is called, it must have only a single argument of type [UI].

### prop
**Signature**: **[UI] prop ([String] name, [Variant] value)**

Sets a property of the [Node] with name `name` to `value`.

### props
**Signature**: **[UI] props ([Dictionary] properties)**

Sets multiple properties from `properties` with each key being the property's name and the value being the property's value.

### motion
**Signature**: **[UI] motion ([Callable] motion_callable)**

Creates a [Motion](api/motion_ref.md) reference if not exists and call `motion_callable` with it to create animations.

### event
**Signature**: **[UI] event ([String] signal_name, [Callable] target)**

Connects the [Node]'s signal of name `signal_name` to the callable `target`.

### queue_update
**Signature**: **[UI] queue_update ()**

Requests this UI to update next frame.

### root_queue_update
**Signature**: **[UI] root_queue_update ()**

Requests this UI's root to update next frame.

### animate_rect
**Signature**: **[UI] animate_rect ([float] speed = 10.0)**

Smoothly animate the [Control]'s rect with linear interpolation with speed `speed`.

### theme_variation
**Signature**: **[UI] theme_variation ([String] theme_type)**

Assign the [Control]'s `theme_type_variation` property to `theme_type`

### shrink_begin
**Signature**: **[UI] shrink_begin ()**

Sets [Control]'s `size_flags_horizontal` and `size_flags_vertical` to `SIZE_SHRINK_BEGIN`

### fill
**Signature**: **[UI] fill ()**

Sets [Control]'s `size_flags_horizontal` and `size_flags_vertical` to `SIZE_FILL`

### expand
**Signature**: **[UI] expand ()**

Sets [Control]'s `size_flags_horizontal` and `size_flags_vertical` to `SIZE_EXPAND`

### expand_fill
**Signature**: **[UI] expand_fill ()**

Sets [Control]'s `size_flags_horizontal` and `size_flags_vertical` to `SIZE_EXPAND`

### shrink_center
**Signature**: **[UI] shrink_center ()**

Sets [Control]'s `size_flags_horizontal` and `size_flags_vertical` to `SIZE_SHRINK_CENTER`

### shrink_end
**Signature**: **[UI] shrink_end ()**

Sets [Control]'s `size_flags_horizontal` and `size_flags_vertical` to `SIZE_SHRINK_END`

### horizontal_shrink_begin
**Signature**: **[UI] horizontal_shrink_begin ()**

Sets [Control]'s `size_flags_horizontal` to `SIZE_SHRINK_BEGIN`

### horizontal_fill
**Signature**: **[UI] horizontal_fill ()**

Sets [Control]'s `size_flags_horizontal` to `SIZE_FILL`

### horizontal_expand
**Signature**: **[UI] horizontal_expand ()**

Sets [Control]'s `size_flags_horizontal` to `SIZE_EXPAND`

### horizontal_expand_fill
**Signature**: **[UI] horizontal_expand_fill ()**

Sets [Control]'s `size_flags_horizontal` to `SIZE_EXPAND`

### horizontal_shrink_center
**Signature**: **[UI] horizontal_shrink_center ()**

Sets [Control]'s `size_flags_horizontal` to `SIZE_SHRINK_CENTER`

### horizontal_shrink_end
**Signature**: **[UI] horizontal_shrink_end ()**

Sets [Control]'s `size_flags_horizontal` to `SIZE_SHRINK_END`

### vertical_shrink_begin
**Signature**: **[UI] vertical_shrink_begin ()**

Sets [Control]'s `size_flags_horizontal` to `SIZE_SHRINK_BEGIN`

### vertical_fill
**Signature**: **[UI] vertical_fill ()**

Sets [Control]'s `size_flags_horizontal` to `SIZE_FILL`

### vertical_expand
**Signature**: **[UI] vertical_expand ()**

Sets [Control]'s `size_flags_horizontal` to `SIZE_EXPAND`

### vertical_expand_fill
**Signature**: **[UI] vertical_expand_fill ()**

Sets [Control]'s `size_flags_horizontal` to `SIZE_EXPAND`

### vertical_shrink_center
**Signature**: **[UI] vertical_shrink_center ()**

Sets [Control]'s `size_flags_horizontal` to `SIZE_SHRINK_CENTER`

### vertical_shrink_end
**Signature**: **[UI] vertical_shrink_end ()**

Sets [Control]'s `size_flags_horizontal` to `SIZE_SHRINK_END`

### full_rect
**Signature**: **[UI] full_rect ()**

Sets [Control]'s `anchor_*` and `offset_*` so that the Control's rect takes the entire parent's rect.

### margin
**Signature**: **[UI] margin ([Variant] unit)**

Sets [Control]'s `anchor_*` and `offset_*` so that the Control's rect has a margin relative to parent.
`unit` can be of type [int] and [float] in pixels and [String] in pixels format: `"64px"` or relative format: `"5%"`

### horizontal_margin
**Signature**: **[UI] horizontal_margin ([Variant] unit)**

Sets [Control]'s `anchor_left`, `anchor_right`, `offset_left` and `offset_right` so that the Control's rect has a margin relative to parent.
`unit` can be of type [int] and [float] in pixels and [String] in pixels format: `"64px"` or relative format: `"5%"`

### vertical_margin
**Signature**: **[UI] vertical_margin ([Variant] unit)**

Sets [Control]'s `anchor_top`, `anchor_bottom`, `offset_top` and `offset_bottom` so that the Control's rect has a margin relative to parent.
`unit` can be of type [int] and [float] in pixels and [String] in pixels format: `"64px"` or relative format: `"5%"`

### left_margin
**Signature**: **[UI] left_margin ([Variant] unit)**

Sets [Control]'s `anchor_left` and `offset_left` so that the Control's rect has a margin relative to parent.
`unit` can be of type [int] and [float] in pixels and [String] in pixels format: `"64px"` or relative format: `"5%"`

### top_margin
**Signature**: **[UI] top_margin ([Variant] unit)**

Sets [Control]'s `anchor_top` and `offset_top` so that the Control's rect has a margin relative to parent.
`unit` can be of type [int] and [float] in pixels and [String] in pixels format: `"64px"` or relative format: `"5%"`

### right_margin
**Signature**: **[UI] right_margin ([Variant] unit)**

Sets [Control]'s `anchor_right` and `offset_right` so that the Control's rect has a margin relative to parent.
`unit` can be of type [int] and [float] in pixels and [String] in pixels format: `"64px"` or relative format: `"5%"`

### bottom_margin
**Signature**: **[UI] bottom_margin ([Variant] unit)**

Sets [Control]'s `anchor_bottom` and `offset_bottom` so that the Control's rect has a margin relative to parent.
`unit` can be of type [int] and [float] in pixels and [String] in pixels format: `"64px"` or relative format: `"5%"`

### create
**Signature**: **[UI] create ([Node] node) static**

Creates a new [UI] binding it to `node`. `Node` must maintain reference to the returned UI and must have a function `ui_update(UI ui)`.

