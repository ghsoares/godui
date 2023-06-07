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
		# Modify the panel theme
		ui.theme()

		# Add a vbox
		ui.add(VBoxContainer).show(func (ui):
			# Add the task name label
			ui.label(task.name)

			# Add a delete button
			ui.button("Delete").sig("pressed", func ():
				# Delete task
				tasks.erase(task.id)

				# Update the interface
				ui.queue_update()
			)
		)
	)

## Creates a new task
func new_task() -> void:
	# Create the task
	var task: Dictionary = {"id": task_id, "name": "New task"}
	
	# Add to tasks array
	tasks[task_id] = task

	# Increment task id
	task_id += 1

	# Queue update
	ui.queue_update()

## Called every frame
func _process(delta: float) -> void:
	# Update the UI
	ui.update(func (ui):
		# Create a vertical section
		ui.add(VBoxContainer).margin(0.0).show(func (ui):
			# Add a label
			ui.label("Tasks")

			# Create a scroll to show the tasks
			ui.add(ScrollContainer).expand_fill().show(func (ui):
				ui.add(HFlowContainer).expand_fill().show(func (ui):
					# For each task, render it
					for t in tasks.values():
						render_task(ui, t)
				)
			)

			# Add a new task button
			ui.button("New task").sig("pressed", new_task)
		)
	)

