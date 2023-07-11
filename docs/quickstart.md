# Quick start

To start using Godui in your 4.x projects, download [the lastest version](https://github.com/ghsoares/godui/releases/latest) or previous versions at [github releases](https://github.com/ghsoares/godui/releases). Then extract the `addons/` folder inside your project.

## Demo project

Godui has a demo project with some UI examples inside `demo/` project in the repository.

## Basic usage

To create an dynamic UI using Godui, first you need to have the base node in the scene tree for the interface, it can be of any Node type, for example a single Control node as root node of the scene.

Attach a script to it and add the following content:

```gdscript
extends Control

## The UI reference
var ui: UI = null

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
	# Add a simple label with a text to it
	ui.add(Label).prop("text", "Hello World!")
```

Breaking it down in important pieces (expecting you already know the basics of GDScript):

- First we declare a variable to store the UI reference: `var ui: UI = null`.

- Second we initialize the interface binding the base node to self: `ui = UI.create(self)`.  
We need to enable frame processing with `set_process(true)` or else the UI won't be able to update when requested or animate things.

- We need to pass Node's notifications to UI from `_notification` function, that way it will be able to update the interface and animate properties, with `if ui: ui.notification(what)` (the conditional is to make sure we aren't calling `notification` from a not yet initialized interface).

- The interface rendering is done inside `ui_process`, UI searchs to this function inside the base node, so make sure to include it. The function receives a single argument, which is the UI itself, in this case it'll be the same as the variable we declared, but more on that later.

- In this simple example we add a single `Label` node with `ui.add(Label)` and assign a text to it with `.prop("text", "Hello World!")`.

Hit play and congratulations! You now have a basic Label added with help of Godui!

## More guides

There are more step-by-step guides to some examples to use Docsify:
- ["Click me" button](guide/click-me-button.md)

