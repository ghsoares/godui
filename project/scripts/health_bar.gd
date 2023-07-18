extends Control

## The UI reference
var ui: UI = null

## The player's image
var image: Texture2D

## Current player health
var health: float = 100.0

## Current player mana
var mana: float = 100.0

## UI for player health
var player_health_ui: UI

## UI for player mana
var player_mana_ui: UI

## Called when the node is ready
func _ready() -> void:
	# Load the player's image
	image = ResourceLoader.load("res://icon.svg")

	# Create the UI reference, then bind it to self
	ui = UI.create(self)

	# Enable processing interface every frame
	set_process(true)

## Called when receiving a notification
func _notification(what: int) -> void:
	# Notify the interface from the Node's notification
	if ui: ui.notification(what)

## Called when player's health changes
func health_changed(health: float) -> void:
	self.health = clamp(health, 0.0, 100.0)
	player_health_ui.queue_update()

## Called when player's mana changes
func mana_changed(mana: float) -> void:
	self.mana = clamp(mana, 0.0, 100.0)
	player_mana_ui.queue_update()

## Renders the player stats for the UI
func render_player_stats(ui: UI) -> void:
	# Create the main container
	var player_stats: UI = ui.hbox()

	# Add the player's image container 
	var img_container: UI = player_stats.add(Control)
	
	# Set it's size to 64 and make it center vertically
	img_container.prop("custom_minimum_size", Vector2(64.0, 64.0))
	img_container.vertical_shrink_center()

	# Add the actual image
	var img: UI = img_container.add(TextureRect)

	# Make it fill the entire parent rect
	img.full_rect()
	img.prop("expand_mode", 1)
	img.prop("stretch_mode", 5)
	img.prop("texture", image)

	# Add a section for health and mana
	var health_mana: UI = player_stats.vbox()
	health_mana.prop("custom_minimum_size", Vector2(256.0, 0.0))
	health_mana.vertical_expand_fill()

	# Add the health progress bar
	health_mana.add(ProgressBar).vertical_expand_fill().prop("size_flags_stretch_ratio", 2.0).show(func (ui):
		# Set reference to this UI
		player_health_ui = ui

		# Set value
		ui.prop("value", health)
	)

	# Add the mana progress bar
	health_mana.add(ProgressBar).vertical_expand_fill().show(func (ui):
		# Set reference to this UI
		player_mana_ui = ui

		# Set value
		ui.prop("value", mana)
	)

## Called to update the interface
func ui_process(ui: UI) -> void:
	# Main container for the UI
	var main: UI = ui.vbox().expand_fill()

	# Render the player stats
	render_player_stats(main)

	# Add a separator
	main.add(HSeparator).horizontal_expand_fill()

	# Add a damage button
	main.button("Damage (-10 HP)").event("pressed", func ():
		health_changed(self.health - 10.0)
	)

	# Add a cast magic button
	main.button("Cast magic (-10 MP)").event("pressed", func ():
		mana_changed(self.mana - 10.0)
	)

	# Add a heal button
	main.button("Heal (+5 HP, -5 MP)").event("pressed", func ():
		health_changed(self.health + 5.0)
		mana_changed(self.mana - 5.0)
	)

	# Add a focus button
	main.button("Focus (+5 MP)").event("pressed", func ():
		mana_changed(self.mana + 5.0)
	)

	
