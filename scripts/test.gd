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
	
	# Add a panel container
	var panel: UI = ui.add(PanelContainer).prop("modulate", Color.WHITE).show(func (ui):
		# Add a vbox
		ui.add(HBoxContainer).show(func (ui):
			# The completed button motion ref (must use a dictionary to pass references between callables)
			var completed_button_motion: Dictionary = {"ref": null}
			
			# Add the completed mark
			var btn: UI = ui.add(Button)
			btn.shrink_center()
			btn.theme_variation("ButtonCheck")
			btn.props({
				"custom_minimum_size": Vector2(16.0, 16.0),
				"pivot_offset": Vector2(8.0, 8.0),
				"button_pressed": task.completed,
				"toggle_mode": true,
				"action_mode": 0
			})
			btn.event("toggled", func (pressed):
				# Set completed
				task.completed = pressed
				print("toggled!")

				# # Motion animation keeps playing infinitely, to change to another animation, you must call 'reset'
				# if completed_button_motion.ref:
				# 	completed_button_motion.ref.reset()

				# Queue UI update
				ui.queue_update()
			)
			# btn.motion(func (motion):
			# 	# Set motion reference
			# 	completed_button_motion.ref = motion
				
			# 	# Play animation on completed
			# 	if task.completed:
			# 		# Scale to 1.25
			# 		motion.prop("scale", func (motion): motion.ease_out(Vector2(1.25, 1.25), 0.1))

			# 		# Play 'shake' animation
			# 		motion.prop("rotation", func (motion):
			# 			motion.repeat(4, func (motion, i):
			# 				var rot: float = deg_to_rad(15.0) * (1.0 - i / 4.0)
			# 				motion.ease_in_out(rot, 0.08).ease_in_out(-rot, 0.08)
			# 			).ease_in_out(0.0, 0.1)
			# 		)

			# 		# Scale back to 1.0
			# 		motion.prop("scale", func (motion): motion.ease_out(Vector2(1.0, 1.0), 0.1))
			# 	# Play animation on not completed
			# 	else:
			# 		# Reset scale and rotation
			# 		motion.parallel(func (motion):
			# 			motion.prop("scale", func (motion): motion.ease_in_out(Vector2(1.0, 1.0), 0.25))
			# 			motion.prop("rotation", func (motion): motion.ease_in_out(0.0, 0.25))
			# 		)
			# )
			# Add the task name line edit
			ui.add(LineEdit).props({
				"text": task.name,
				"expand_to_text_length": true,
			}).horizontal_expand_fill().event("text_changed", func (new_text):
				# Set task name
				task.name = new_text

				# Queue UI update
				ui.queue_update()
			)

			# Add a delete button
			button(ui, "Delete").prop("disabled", task.removed).event("pressed", func ():
				# Mark as removed
				task.removed = true

				# Erase for real now
				tasks.erase(task.id)

				# # Play panel motion
				# if panel_motion.ref: panel_motion.ref.reset()

				# Queue UI update
				ui.queue_update()
			)

			label(ui, "ID: %s" % ui.ref().get_instance_id())
		)
	)

	# panel.motion(func (motion):
	# 	panel_motion.ref = motion

	# 	# Play animation on removed
	# 	if task.removed:
	# 		motion.prop("modulate", func (motion):
	# 			motion.ease_linear(Color.TRANSPARENT, 0.5)
	# 		).callback(func ():
	# 			# Erase for real now
	# 			tasks.erase(task.id)

	# 			# Queue UI update
	# 			ui.queue_update()
	# 		)
	# )

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
	ui.add(PanelContainer).theme_variation("BaseContainer").full_rect().show(func (ui):
		# Create a vertical section
		ui.add(VBoxContainer).show(func (ui):
			# Add a label
			label(ui, "Tasks")

			# Create a scroll to show the tasks
			ui.add(ScrollContainer).expand_fill().show(func (ui):
				ui.add(VBoxContainer).expand_fill().show(func (ui):
					# For each task, render it
					for t in tasks.values():
						render_task(ui, t)
				)
			)

			# Add a new task button
			button(ui, "New task").prop("action_mode", 0).event("pressed", new_task)
		)
	)




