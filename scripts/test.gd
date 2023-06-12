extends Control

## The UI reference
var ui: UI

## Current tasks
var tasks: Dictionary

## Current task id
var task_id: int = 0

## Called when ready
func _ready() -> void:
	ui = UI.new(self)

## Renders a single task
func render_task(ui: UI, task: Dictionary) -> void:
	# Add a panel container
	ui.add(PanelContainer).show(func (ui):
		# Add a vbox
		ui.add(HBoxContainer).show(func (ui):
			# The color rect motion ref (must use a dictionary to pass references between callables)
			var color_rect_motion: Dictionary = {"ref": null}

			# Add the completed mark
			var btn: UIRef = ui.add(Button).shrink_center().theme_variation("ButtonCheck").props({
				"custom_minimum_size": Vector2(16.0, 16.0),
				"button_pressed": task.completed,
				"toggle_mode": true,
				"action_mode": 0
			}).sig("toggled", func (pressed):
				# Set completed
				task.completed = pressed

				# Reset animation
				if color_rect_motion.ref:
					color_rect_motion.ref.reset()

				# Queue UI update
				ui.queue_update()
			)

			# Add a color rect
			ui.add(ColorRect).props({
				"custom_minimum_size": Vector2(16.0, 16.0),
				"pivot_offset": Vector2(8.0, 8.0)
			}).shrink_center().motion(func (motion):
				# Set the motion reference
				color_rect_motion.ref = motion
				
				# Run one animation when completed
				if task.completed:
					motion.parallel(func (motion):
						# Transition to green
						motion.prop("modulate", func (motion):
							motion.ease_out(Color.GREEN, 0.25)
						)
						# Repeat rotate and scale 3 times
						motion.repeat(3, func (motion, _idx): 
							motion.parallel(func (motion):
								# Rotate 90 degrees and snap back to 0 when finishing rotation
								motion.prop("rotation", func (motion):
									motion.ease_out(PI * 0.5, 0.25, 3.0).frame(0.0).wait(0.75)
								)
								# Scale to 1.25 then scale back to 1.0 (bounce)
								motion.prop("scale", func (motion):
									motion.ease_out(Vector2(1.25, 1.25), 0.1).ease_in_out(Vector2(1.0, 1.0), 0.7)
								)
							)
						)
					)
				# And other animation when not completed
				else:
					motion.parallel(func (motion):
						# Transition to white
						motion.prop("modulate", func (motion):
							motion.ease_in_out(Color.WHITE, 0.5)
						)
						# Rotate back to 0.0
						motion.prop("rotation", func (motion):
							motion.ease_in_out(0.0, 0.5)
						)
						# Scale back to 0.0
						motion.prop("scale", func (motion):
							motion.ease_in_out(Vector2(1.0, 1.0), 0.5)
						)
					)
			)

			# Add the task name line edit
			ui.add(LineEdit).props({
				"text": task.name,
				"expand_to_text_length": true,
			}).horizontal_expand_fill().sig("text_changed", func (new_text):
				# Set task name
				task.name = new_text

				# Queue UI update
				ui.queue_update()
			)

			# Add a delete button
			ui.button("Delete").sig("pressed", func ():
				# Delete task
				tasks.erase(task.id)

				# Queue UI update
				ui.queue_update()
			)
		)
	)

## Creates a new task
func new_task() -> void:
	# Create the task
	var task: Dictionary = {"id": task_id, "name": "New task", "completed": false}
	
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
			ui.label("Tasks")

			# Create a scroll to show the tasks
			ui.add(ScrollContainer).expand_fill().show(func (ui):
				ui.add(VBoxContainer).expand_fill().show(func (ui):
					# For each task, render it
					for t in tasks.values():
						render_task(ui, t)
				)
			)

			# Add a new task button
			ui.button("New task").prop("action_mode", 0).sig("pressed", new_task)
		)
	)

