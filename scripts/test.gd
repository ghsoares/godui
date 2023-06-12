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
			var btn: UIRef = ui.add(Button).theme_variation(
				"ButtonCheck"
			).prop(
				"button_pressed", task.completed
			).sig("toggled", func (pressed):
				# Set completed as pressed
				task.completed = pressed

				# Queue UI update
				ui.queue_update()
			).motion(func (motion):
				motion.prop("modulate", func (motion):
					motion.initial(Color.WHITE)
					motion.linear(Color.RED, 0.25)
					motion.linear(Color.GREEN, 0.25)
					motion.linear(Color.BLUE, 0.25)
					motion.loop(0.25)
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
			ui.button("New task").sig("pressed", new_task)
		)
	)

