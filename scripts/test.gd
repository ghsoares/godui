@tool
extends Control

## The UI reference
var ui: UI
var ui_ref: WeakRef

## The ms label
var ms_label: Label

## The task list UI reference
var task_list_ui: UI

## Current tasks
var tasks: Dictionary

## Current MS
var ms: float = 0.0
var _ms: float = 0.0

## Current task id
var task_id: int = 0

## Called when receiving a notification
func _notification(what: int) -> void:
	match what:
		NOTIFICATION_READY:
			ui = UI.create(self)
			ui_ref = weakref(ui)
			set_process(true)

			get_script().changed.connect(on_script_changed)
			script_changed.connect(on_script_changed)
		NOTIFICATION_PROCESS:
			if _ms > ms:
				ms = _ms
			else:
				ms += (_ms - ms) * clamp(get_process_delta_time() * 4.0, 0.0, 1.0)
			if ms_label: 
				ms_label.text = "%.2f ms" % ms

	if ui: 
		var start: int = Time.get_ticks_usec()
		ui.notification(what)
		var elapsed: int = Time.get_ticks_usec() - start
		_ms = elapsed * 0.001

func on_script_changed() -> void:
	print("Changed!")
	pass

## Called to create a label
func label(ui: UI, text: String) -> UI:
	return ui.add(Label).prop("text", text)

## Called to create a button
func button(ui: UI, text: String) -> UI:
	return ui.add(Button).prop("text", text)

## Renders a single task
func render_task(ui: UI, task: Dictionary) -> void:
	# Add a panel container
	ui.add(PanelContainer, "Task ID %s" % task.id).show(func (ui):
		# The panel motion reference
		var panel_motion: Dictionary = {"ref": null}

		# The motion reference for the completed button
		var completed_button_motion: Dictionary = {"ref": null}

		# Grab the reference to the panel container
		var panel_ui: UI = ui
	
		# Set base color to white
		ui.prop("modulate", Color.WHITE)

		# Animate rect with 16 as speed
		ui.animate_rect(24.0)

		# Motion to animate and fade to invisible
		ui.motion(func (motion):
			# Set panel motion reference
			panel_motion.ref = motion

			# Play animation on removed
			if task.removed:
				# Fade to invisible
				motion.prop("modulate").from_current().linear(Color.TRANSPARENT, 0.2).callback(func ():
					# Erase for real now
					tasks.erase(task.id)

					# Queue task list UI update
					task_list_ui.queue_update()
				)
		)

		# Add a hbox
		var hbox: UI = ui.add(HBoxContainer)

		# Add a completed mark
		var completed_button: UI = hbox.add(Button).shrink_center().theme_variation("ButtonCheck").props({
			"custom_minimum_size": Vector2(16.0, 16.0),
			"pivot_offset": Vector2(8.0, 8.0),
			"button_pressed": task.completed,
			"toggle_mode": true,
			"action_mode": 0
		})

		# Add toggled event
		completed_button.event("toggled", func (pressed):
			# Set completed
			task.completed = pressed

			# Motion animation keeps playing infinitely, to change to another animation, you must call 'reset'
			if completed_button_motion.ref:
				completed_button_motion.ref.reset()

			# Queue container update
			panel_ui.queue_update()
		)

		# Add juicy animation on completed
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
						motion.wait(0.08)
						motion.prop("scale:y", true).from_current().ease_out(3.0, 0.1)
						motion.prop("scale:x", true).ease_out(1.5, 0.1)
						motion.wait(0.08)
						motion.prop("scale:y", true).ease_out(1.5, 0.1)
					)

					# Play 'shake' animation
					motion.prop("rotation").from_current().pulse(deg_to_rad(45.0) * (randf() * 2.0 - 1.0), 0.25)
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

		# Add the task name LineEdit
		hbox.add(LineEdit).show(func (ui):
			# Set current text to the task's name
			ui.prop("text", task.name)

			# Make it fill horizontally
			ui.horizontal_expand_fill()

			# Add a text_changed signal
			ui.event("text_changed", func (new_text):
				# Set task name
				task.name = new_text
			)
		)

		# Add a delete button
		button(hbox, "Delete").show(func (ui):
			# Make it disabled to avoid removal repetition
			ui.prop("disabled", task.removed)

			# Add the pressed event
			ui.event("pressed", func ():
				# Mark as removed
				task.removed = true

				# Play panel motion
				if panel_motion.ref: panel_motion.ref.reset()

				# Queue container update
				panel_ui.queue_update()
			)
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
	
	# Queue task list update
	task_list_ui.queue_update()

## Called to update the ui
func ui_process(ui: UI) -> void:
	# The main panel with vbox
	var main_ui: UI = ui.add(PanelContainer).full_rect().theme_variation("BaseContainer").add(VBoxContainer)

	# Add a simple ms label to main panel
	ms_label = label(main_ui, "0.00 ms").ref()

	# Add a simple label to main panel
	label(main_ui, "Tasks")

	# Add a scroll section with vbox
	task_list_ui = main_ui.add(ScrollContainer).expand_fill().add(VBoxContainer).expand_fill().show(func (ui):
		# For each task, render it to the scroll
		for task in tasks.values():
			render_task(ui, task)
	)

	# Add a new task button to main panel
	button(main_ui, "New task").show(func (ui):
		# Set action mode to 0 (emit signal on mouse down)
		ui.prop("action_mode", 0)

		# Add the pressed signal
		ui.event("pressed", self.new_task)
	)




