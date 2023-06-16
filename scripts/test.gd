extends Control

## The UI reference
var ui: UI

## Current tasks
var tasks: Dictionary

## Current task id
var task_id: int = 0

## Called when ready
func _ready() -> void:
	ui = UI.create(self)

## Called to create a label
func label(ui: UI, text: String) -> UI:
	return ui.add(Label).prop("text", text)

## Called to create a button
func button(ui: UI, text: String) -> UI:
	return ui.add(Button).prop("text", text)

## Renders a single task
func render_task(ui: UI, task: Dictionary) -> void:
	# The panel motion reference
	var panel_motion: Dictionary = {"ref": null}
	var completed_button_motion: Dictionary = {"ref": null}
	
	# Add a panel container
	var main_panel: UI = ui.add(PanelContainer, "Task ID %s" % task.id, false).prop("modulate", Color.WHITE).animate_rect(16.0)
	
	# Add a hbox inside container
	var main_panel_hbox: UI = main_panel.add(HBoxContainer)

	# Add the completed mark, make it shrink to center, add ButtonCheck as theme variation and set some properties
	var completed_button: UI = main_panel_hbox.add(Button).shrink_center().theme_variation("ButtonCheck").props({
		"custom_minimum_size": Vector2(16.0, 16.0),
		"pivot_offset": Vector2(8.0, 8.0),
		"button_pressed": task.completed,
		"toggle_mode": true,
		"action_mode": 0
	}).event("toggled", func (pressed):
		# Set completed
		task.completed = pressed

		# Motion animation keeps playing infinitely, to change to another animation, you must call 'reset'
		if completed_button_motion.ref:
			completed_button_motion.ref.reset()

		# Queue UI update
		ui.queue_update()
	)

	# Add the task name LineEdit
	main_panel_hbox.add(LineEdit).prop("text", task.name).horizontal_expand_fill().event("text_changed", func (new_text):
		# Set task name
		task.name = new_text

		# Queue UI update
		ui.queue_update()
	)

	# Add a delete button
	button(main_panel_hbox, "Delete").prop("disabled", task.removed).event("pressed", func ():
		# Mark as removed
		task.removed = true

		# Play panel motion
		if panel_motion.ref: panel_motion.ref.reset()

		# Queue UI update
		ui.queue_update()
	)
	
	completed_button.motion(func (motion):
		# Set motion reference
		completed_button_motion.ref = motion
		
		# Play animation on completed
		if task.completed:
			# Paralelly animate tracks
			motion.parallel(func (motion):
				# Another parallel track to animate both axis individually of the scale property
				motion.parallel(func (motion):
					motion.prop("scale:x", true).from_current().ease_out(3.0, 0.1)
					motion.wait(0.1)
					motion.prop("scale:y", true).from_current().ease_out(3.0, 0.1)
					motion.prop("scale:x", true).ease_out(1.5, 0.1)
					motion.wait(0.1)
					motion.prop("scale:y", true).ease_out(1.5, 0.1)
				)

				# Play 'shake' animation
				motion.prop("rotation").from_current().shake(deg_to_rad(45.0) * (randf() * 2.0 - 1.0), 0.5, 3.0)
			)

			# Reset scale and rotation
			motion.parallel(func (motion):
				motion.prop("scale:x").ease_in_out(1.0, 0.25)
				motion.prop("scale:y").ease_in_out(1.0, 0.25)
				motion.prop("rotation").ease_in_out(0.0, 0.25)
			)
		# Play animation on not completed
		else:
			# Animate two properties parallelly
			motion.parallel(func (motion):
				# Reset both scale and rotation to zero
				motion.prop("scale").from_current().ease_in_out(Vector2(1.0, 1.0), 0.25)
				motion.prop("rotation").from_current().ease_in_out(0.0, 0.25)
			)
	)

	main_panel.motion(func (motion):
		panel_motion.ref = motion

		# Play animation on removed
		if task.removed:
			motion.prop("modulate").from_current().linear(Color.TRANSPARENT, 0.2).callback(func ():
				# Erase for real now
				tasks.erase(task.id)

				# Queue UI update
				ui.queue_update()
			)
	)

## Creates a new task
func new_task() -> void:
	# Create the task
	var task: Dictionary = {"id": task_id, "name": "New task", "completed": false, "removed": false}
	
	# Add to tasks array
	tasks[task_id] = task
	
	# Increment task id
	task_id += 1
	
	# Queue update
	ui.queue_update()

## Called to update the ui
func ui_process(ui: UI) -> void:
	# Main panel
	var main_panel: UI = ui.add(PanelContainer).theme_variation("BaseContainer").full_rect().add(VBoxContainer)

	# Add a simple label to main panel
	label(main_panel, "Tasks")

	# Add a vertical section to main panel
	var scroll: UI = main_panel.add(ScrollContainer).expand_fill().add(VBoxContainer).expand_fill()

	# For each task, render it to the scroll
	for task in tasks.values():
		render_task(scroll, task)
	
	# Add a new task button to main panel
	button(main_panel, "New task").prop("action_mode", 0).event("pressed", new_task)



