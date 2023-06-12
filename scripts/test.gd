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
			# Add the completed mark
			var btn: UIRef = ui.add(Button).theme_variation("ButtonCheck").props({
				"button_pressed": task.completed,
				"toggle_mode": true,
				"action_mode": 0
			}).sig("toggled", func (pressed):
				# Set completed as pressed
				task.completed = pressed

				# Queue UI update
				ui.queue_update()
			)

			# Add a color rect
			ui.add(ColorRect).props({
				"custom_minimum_size": Vector2(16.0, 16.0),
				"pivot_offset": Vector2(8.0, 8.0)
			}).shrink_center().motion(func (motion):
				if task.completed:
					motion.parallel(func (motion):
						motion.prop("modulate", func (motion):
							motion.ease_out(Color.GREEN, 0.25)
						).repeat(3, func (motion, _idx): 
							motion.prop("rotation", func (motion):
								motion.ease_out(PI * 0.5, 0.25).wait(0.75)
							)
						)
					)
				else:
					motion.parallel(func (motion):
						motion.prop("modulate", func (motion):
							motion.ease_out(Color.WHITE, 0.25)
						).prop("rotation", func (motion):
							motion.ease_out(0.0, 0.25)
						)
					)
				motion.reset()
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
			ui.button("New task").sig("pressed", new_task)
		)
	)

