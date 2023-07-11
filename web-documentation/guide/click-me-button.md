# "Click me" button

(This guide builds on top of [Godui's basic usage](quickstart.md#basic-usage))

Let's try a more complete example, let's create a <span class-method-ref>Button</span> which tells how many times you clicked it.

Let's being by declaring a variable to store the clicks:

```gdscript
# The number of clicks registered by the button
var clicks: int = 0
```

Now inside `ui_process` function, replace the label with a button:

```gdscript
## Called to update the interface
func ui_process(ui: UI) -> void:
	# Add a button
	var button: UI = ui.add(Button)
```

The neat part of Godui is that every added node is also a UI, so you can nest UIs inside UIs to build the entire scene tree, but let's stick with simple steps first.

Let's conditionally add a text to it, first saying `click me!` without any click registered, then we go for `you clicked me {} times!` after first click:

```gdscript
# Display a simple text with no clicks
if clicks == 0:
	button.prop("text", "click me!")
# Else display the number of clicks
else:
	# We're using format string to insert the click count inside the text
	button.prop("text", "you clicked %d times!" % clicks)
```

Now for the important part, we need to increment `clicks` every time we click the button, to do this we need to connect to the button's `pressed` signal:

```gdscript
# Here we're connecting to the button's "pressed" signal 
# and increment "clicks" by 1 every time the signal is emitted
button.event("pressed", func (): clicks += 1)
```

If you hit play the first thing you see is the button with the text `click me!` as expected, now when pressing the button... Nothing happens, why?

The UI isn't updated every frame or detects when a variable changes, instead it only update when explicitaly requested, that way you are free to choose when the interface must be updated and which parts need to be updated. To do this we have two functions: `UI.queue_update` and `UI.root_queue_update`, both requests a UI to update, but `root_queue_update` requests the update from the root UI (the one we create from script assigning `self` as base Node), which used by nested UIs. For this example we can use `queue_update` directly:

```gdscript
# Here we're connecting to the button's "pressed" signal 
button.event("pressed", func (): 
	# Increment "clicks" by 1 every time the signal is emitted
	clicks += 1

	# Requests the UI to update
	ui.queue_update()
)
```

Now, the first time you press the button, you should see the text `you clicked 1 times!`, when you press again you should see `you clicked 2 times!` and so on. 

Congratulations! You now have a button with dynamic text added with help of Godui! :tada:

The complete code of this guide:

```gdscript
extends Control

## The UI reference
var ui: UI = null

# The number of clicks registered by the button
var clicks: int = 0

## Called when the node is ready
func _ready() -> void:
	# Create the UI reference, then bind it to self
	ui = UI.create(self)

	# Enable processing interface every frame
	set_process(true)

## Called when receiving a notification
func _notification(what: int) -> void:
	# Notify the interface from the Node's notification
	if ui: ui.notification(what)

## Called to update the interface
func ui_process(ui: UI) -> void:
	# Add a button
	var button: UI = ui.add(Button)

	# Display a simple text with no clicks
	if clicks == 0:
		button.prop("text", "click me!")
	# Else display the number of clicks
	else:
		# We're using format string to insert the click count inside the text
		button.prop("text", "you clicked %d times!" % clicks)
	
	# Here we're connecting to the button's "pressed" signal 
	button.event("pressed", func (): 
		# Increment "clicks" by 1 every time the signal is emitted
		clicks += 1

		# Requests the UI to update
		ui.queue_update()
	)
```