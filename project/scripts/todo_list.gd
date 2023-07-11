extends Control

## The UI reference
var ui: UI = null

## An dictionary storing currently added tasks
var tasks: Dictionary = {}

## An counter for the tasks's ids, used for indexing later
var task_id: int = 0

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

## Creates a single task
func task_create() -> void:
	# Create a dictioary describing the task name, id and if it's completed
	var task: Dictionary = {
		"name": "New task",
		"id": task_id,
		"completed": false
	}

	# Add to the tasks dictionary using task's id as key
	# and increment `task_id` by one for the next task's id
	tasks[task_id] = task
	task_id += 1

## Marks a single task as completed or not completed
func task_set_completed(id: int, completed: bool = true) -> void:
	# Get the task by it's id
	var task: Dictionary = tasks[id]

	# Mark `completed` as the passed argument
	task.completed = completed

## Sets the task's name
func task_set_name(id: int, name: String) -> void:
	# Get the task by it's id
	var task: Dictionary = tasks[id]

	# Set `name` to the passed argument
	task.name = name

## Deletes a single task by id
func task_delete(id: int) -> void:
	# Simply erase from tasks dictionary
	tasks.erase(id)

## Display a single task to the UI
func show_task(ui: UI, task: Dictionary) -> void:
	# Let's contain the task inside a PanelContainer with a HBoxContainer,
	# you can chain `add` methods to reduce variables. Also add a smooth
	# transition to the panel container inside the list.
	var task_panel: UI = ui.add(PanelContainer, task.id).animate_rect().add(HBoxContainer)

	# Let's add content dynamically inside `show`
	task_panel.show(func (ui):
		# Let's add a CheckBox for the completed property
		var task_completed_ui: UI = task_panel.add(CheckBox).prop("pressed", task.completed)

		# Set the checkbox's pivot_offset to center
		task_completed_ui.prop("pivot_offset", task_completed_ui.ref().size * 0.5)

		# Store the task completed reference to reset it when toggled
		var task_completed_motion: Dictionary = {"ref": null}

		# Connect the `toggled` and set the task's completed value
		task_completed_ui.event("toggled", func (completed: bool):
			# Set the task `completed` value to the argument passed by the signal
			task.completed = completed

			# We need to reset the motion to play another animation
			if task_completed_motion.ref:
				task_completed_motion.ref.reset()

			# Update the panel's UI
			ui.queue_update()
		)

		# Add a animation to the checkbox
		task_completed_ui.motion(func (motion: MotionRef):
			# Set the motion reference
			task_completed_motion.ref = motion

			# Rotates in a direction when completed
			if task.completed:
				# Let's animate two properties parallely
				motion.parallel(func (motion):
					# Let's animate the `rotation` property
					motion.prop("rotation")

					# Start from the node's current rotation
					motion.from_current()

					# Rotate to a full turn during 500 milliseconds using
					# easing out transition
					motion.ease_out(TAU, 0.5)

					# Let's also animate the `scale` property
					motion.prop("scale")

					# Start from the node's current scale
					motion.from_current()

					# Scale to 1.5 during 500 milliseconds using
					# easing out transition
					motion.ease_out(Vector2(1.5, 1.5), 0.5)
				)

				# After above parallel animation finishes, let's animate scale
				motion.prop("scale")

				# Scale to 1.0 during 500 milliseconds using
				# easing in-out transition
				motion.ease_in_out(Vector2(1.0, 1.0), 0.5)

				# Also snap rotation back to zero (it's the same as 360)
				motion.prop("rotation").frame(0.0)
			# Rotates and scales back to zero when not completed
			else:
				# Let's animate two properties parallely
				motion.parallel(func (motion):
					# Let's animate the `rotation` property
					motion.prop("rotation")

					# Start from the node's current rotation
					motion.from_current()

					# Rotate to zero during 500 milliseconds using
					# easing in-out transition
					motion.ease_in_out(0.0, 0.5)

					# Let's also animate the `scale` property
					motion.prop("scale")

					# Start from the node's current scale
					motion.from_current()

					# Scale to 1.0 during 500 milliseconds using
					# easing in-out transition
					motion.ease_in_out(Vector2(1.0, 1.0), 0.5)
				)
		)

		# Now a single LineEdit for the task description, also make it fill
		# the remaining width from the HBoxContainer
		var task_name_ui: UI = task_panel.line_edit(task.name).horizontal_expand_fill()

		# Connect the `text_changed` signal to change the task's name
		task_name_ui.event("text_changed", func (name: String):
			# Set the task `name` value to the argument passed by the signal
			task.name = name

			# Update the panel's UI
			ui.queue_update()
		)

		# Lastly a button to delete the task
		var task_delete_ui: UI = task_panel.button("delete")

		# Connect the `pressed` signal to delete the task when pressed
		task_delete_ui.event("pressed", func ():
			# Delete the task from ID
			task_delete(task.id)

			# Here we have no option other than update the entire UI
			# to update the task list
			ui.root_queue_update()
		)
	)

## Called to update the interface
func ui_process(ui: UI) -> void:
	# Let's contain multiple tasks inside a VBoxContainer, also make
	# the VBoxContainer fill the entire UI with margin of 8 pixels
	var tasks_ui: UI = ui.add(VBoxContainer).margin(8.0)

	# For each value in the tasks dictionary, we call the `show_task` function
	for task in tasks.values():
		show_task(tasks_ui, task)
	
	# Add a button to add new tasks and add smooth transition inside the
	# task list
	var new_task_ui: UI = tasks_ui.button("New task").animate_rect()

	# Connect the `pressed` signal to add a new task when pressed
	new_task_ui.event("pressed", func ():
		# Create a new task
		task_create()

		# Update the root UI
		ui.root_queue_update()
	)



