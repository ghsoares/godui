#include "ui.h"
#include "util.h"
#include "unit.h"

#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/random_number_generator.hpp>
#include <godot_cpp/classes/gd_script.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/time.hpp>
#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/theme_db.hpp>

using namespace godot;

HashMap<String, Object *> UI::builtin_scripts = HashMap<String, Object *>();

void UI::_notification(int p_what) {
	switch (p_what) {
		case Node::NOTIFICATION_ENTER_TREE: {
			RenderingServer::get_singleton()->connect("frame_pre_draw", callable_mp(this, &UI::before_draw));
		} break;
		case Node::NOTIFICATION_EXIT_TREE: {
			RenderingServer::get_singleton()->disconnect("frame_pre_draw", callable_mp(this, &UI::before_draw));
		} break;
		case Node::NOTIFICATION_PROCESS: {
			check_update();
			idle_update(node->get_process_delta_time());
		} break;
	}
}

void UI::before_draw() {
	draw_update(node->get_process_delta_time());
}

void UI::clear_children() {
	for (UITypeCollection::Iterator type = types.begin(); type; ++type) {
		for (UIChildrenCollection::Iterator child = type->value.children.begin(); child; ++child) {
			child->value->clear_children();
			if (child->value->inside)
				node->remove_child(child->value->node);
			child->value->node->queue_free();
		}
	}
}

Ref<UI> UI::debug(bool p_enabled) {
	if (p_enabled == debug_canvas_item.is_valid()) return this;
	ERR_FAIL_COND_V_MSG(!Object::cast_to<Control>(node), this, "Node must inherit Control");

	Control *control = Object::cast_to<Control>(node);

	if (p_enabled) {
		debug_canvas_item = RenderingServer::get_singleton()->canvas_item_create();
		RenderingServer::get_singleton()->canvas_item_set_parent(debug_canvas_item, control->get_canvas_item());

		if (debug_font.is_null()) {
			debug_font = ThemeDB::get_singleton()->get_fallback_font();
		}
	} else {
		RenderingServer::get_singleton()->free_rid(debug_canvas_item);
		debug_canvas_item = RID();
	}

	return this;
}

void UI::check_update() {
	for (UITypeCollection::Iterator type = types.begin(); type; ++type) {
		for (UIChildrenCollection::Iterator child = type->value.children.begin(); child; ++child) {
			child->value->check_update();
		}
	}

	if (repaint) {
		pre_update();
		ui_process();
		post_update();
	}
}

void UI::pre_update() {
	for (UITypeCollection::Iterator type = types.begin(); type; ++type) {
		type->value.idx = 0;
		for (UIChildrenCollection::Iterator child = type->value.children.begin(); child; ++child) {
			if (child->value->inside) child->value->deletion = true;
			child->value->pre_update();
		}
	}
	
	for (HashMap<String, SignalInfo>::Iterator signal = signals.begin(); signal; ++signal) {
		if (!signal->value.target.is_null()) {
			signal->value.disconnect = true;
		}
	}

	if (node_motion.is_valid()) {
		node_motion->clear();
	}

	node->set_block_signals(true);

	child_idx = 0;
}

void UI::ui_process() {
	for (UITypeCollection::Iterator type = types.begin(); type; ++type) {
		type->value.idx = 0;
		for (UIChildrenCollection::Iterator child = type->value.children.begin(); child; ++child) {
			child->value->ui_process();
		}
	}
	repaint = false;
	if (!update_callable.is_null()) update_callable.call(this);
}

void UI::post_update() {
	for (UITypeCollection::Iterator type = types.begin(); type; ++type) {
		for (UIChildrenCollection::Iterator child = type->value.children.begin(); child; ++child) {
			child->value->post_update();
			if (child->value->deletion) {
				child->value->remove();
				if (!child->value->persist) {
					child->value->del();
					type->value.children.erase(child->key);
				}
			}
		}
	}

	for (HashMap<String, SignalInfo>::Iterator signal = signals.begin(); signal; ++signal) {
		if (signal->value.disconnect) {
			node->disconnect(signal->key, signal->value.target);
			signal->value.target = Callable();
			signal->value.disconnect = false;
		}
	}
	
	node->set_block_signals(false);

	debug_prev_update_elapsed = 0.0;
}

void UI::remove() {
	parent->node->remove_child(node);

	for (HashMap<String, SignalInfo>::Iterator signal = signals.begin(); signal; ++signal) {
		if (!signal->value.target.is_null()) {
			node->disconnect(signal->key, signal->value.target);
		}
	}

	if (node_motion.is_valid()) {
		node_motion->clear();
		node_motion->reset();
	}

	signals.clear();

	inside = false;
	deletion = false;
}

void UI::del() {
	for (UITypeCollection::Iterator type = types.begin(); type; ++type) {
		for (UIChildrenCollection::Iterator child = type->value.children.begin(); child; ++child) {
			child->value->del();
			type->value.children.erase(child->key);
		}
	}

	node->queue_free();
	node = nullptr;
}

void UI::idle_update(float p_delta) {
	for (UITypeCollection::Iterator type = types.begin(); type; ++type) {
		for (UIChildrenCollection::Iterator child = type->value.children.begin(); child; ++child) {
			child->value->idle_update(p_delta);
		}
	}

	if (inside && node_motion.is_valid()) {
		node_motion->time += p_delta;
		node_motion->animate();
	}
}

void UI::draw_update(float p_delta) {
	for (UITypeCollection::Iterator type = types.begin(); type; ++type) {
		for (UIChildrenCollection::Iterator child = type->value.children.begin(); child; ++child) {
			child->value->draw_update(p_delta);
		}
	}

	debug_prev_update_elapsed += p_delta / 0.25;

	if (rect_animation_speed > 0.0) {
		Control *control = Object::cast_to<Control>(node);
		Rect2 curr = control->get_rect();
		Transform2D tr = control->get_transform();
		RID rid = control->get_canvas_item();
		float t = p_delta * rect_animation_speed;
		t = t < 0.0 ? 0.0 : (t > 1.0 ? 1.0 : t);

		if (rect_current.size.x < 0.0) {
			rect_current = curr;
		}

		rect_current.position += (curr.position - rect_current.position) * t;
		rect_current.size += (curr.size - rect_current.size) * t;

		if (ABS(rect_current.position.x - curr.position.x) < 0.5) rect_current.position.x = curr.position.x;
		if (ABS(rect_current.position.y - curr.position.y) < 0.5) rect_current.position.y = curr.position.y;
		if (ABS(rect_current.size.x - curr.size.x) < 0.5) rect_current.size.x = curr.size.x;
		if (ABS(rect_current.size.y - curr.size.y) < 0.5) rect_current.size.y = curr.size.y;

		Vector2 delta_pos = rect_current.position - curr.position;
		Vector2 delta_scale = rect_current.size / curr.size;

		tr *= Transform2D(Vector2(delta_scale.x, 0.0), Vector2(0.0, delta_scale.y), delta_pos);

		RenderingServer::get_singleton()->canvas_item_set_transform(rid, tr);
	}

	if (node_draw.is_valid()) {
		node_draw->time += p_delta;
		node_draw->delta = p_delta;
		if (node_draw->queued_redraw) {
			node_draw->node->queue_redraw();
		}
	}

	if (debug_canvas_item.is_valid()) {
		RenderingServer::get_singleton()->canvas_item_clear(debug_canvas_item);

		Control *control = Object::cast_to<Control>(node);
		if (control) {
			Vector2 a = Vector2();
			Vector2 b = control->get_size();
			Color rect_color = debug_color;
			Color update_color = debug_update_color;
			update_color.a *= CLAMP(1.0 - debug_prev_update_elapsed, 0.0, 1.0);

			RenderingServer::get_singleton()->canvas_item_set_draw_index(debug_canvas_item, node->get_child_count());
			RenderingServer::get_singleton()->canvas_item_add_rect(
				debug_canvas_item,
				Rect2(a, b),
				update_color
			);
			RenderingServer::get_singleton()->canvas_item_add_line(
				debug_canvas_item,
				Vector2(a.x, a.y),
				Vector2(b.x, a.y),
				rect_color,
				3.0
			);
			RenderingServer::get_singleton()->canvas_item_add_line(
				debug_canvas_item,
				Vector2(b.x, a.y),
				Vector2(b.x, b.y),
				rect_color,
				3.0
			);
			RenderingServer::get_singleton()->canvas_item_add_line(
				debug_canvas_item,
				Vector2(b.x, b.y),
				Vector2(a.x, b.y),
				rect_color,
				3.0
			);
			RenderingServer::get_singleton()->canvas_item_add_line(
				debug_canvas_item,
				Vector2(a.x, b.y),
				Vector2(a.x, a.y),
				rect_color,
				3.0
			);

			String debug_text = vformat("IID: %s", node->get_instance_id());
			Vector2 size = debug_font->get_string_size(debug_text, (HorizontalAlignment)0, -1, 12);

			debug_font->draw_string(
				debug_canvas_item,
				Vector2(0.0, size.y),
				debug_text,
				(HorizontalAlignment)0,
				-1, 12
			);
		}
	}
}

void UI::set_builtin_classes(const Dictionary &p_dict) {
	UI::builtin_scripts.clear();
	Array keys = p_dict.keys();
	for (int i = 0; i < keys.size(); i++) {
		Variant k = keys[i];
		ERR_FAIL_COND(k.get_type() != Variant::Type::STRING && k.get_type() != Variant::Type::STRING_NAME);
		String name = k;
		Object *script = p_dict[name];
		UI::builtin_scripts.insert(name, script);
	}
}

Object *UI::get_builtin_class(const String &p_class) {
	ERR_FAIL_COND_V_MSG(
		!UI::builtin_scripts.has(p_class),
		nullptr,
		vformat("Couldn't get class of type '%s'", p_class)
	);

	return UI::builtin_scripts.get(p_class);
}

Ref<UI> UI::add(Object *p_type, const Variant &p_key, bool p_persist) {
	ERR_FAIL_COND_V_MSG(!p_type, nullptr, "Type is null");
	ERR_FAIL_COND_V_MSG(
		!Object::cast_to<PackedScene>(p_type) && !p_type->has_method("new"),
		nullptr,
		"Type must be a PackedScene, native class or a script"
	);

	uint64_t type_key = p_type->get_instance_id();
	UITypeCollection::Iterator type = types.find(type_key);
	if (!type) {
		type = types.insert(type_key, UINodeCollection());
	}

	String index;

	if (p_key.get_type() != Variant::NIL) {
		index = p_key.stringify();
		ERR_FAIL_COND_V_MSG(index.begins_with("__godui_ui:"), nullptr, "Provided key can't begin with reserved string '__godui_ui:'");
	} else {
		index = vformat("__godui_id:%d", type->value.idx++);
	}

	UIChildrenCollection::Iterator ref = type->value.children.find(index);
	if (!ref) {
		PackedScene *packed_scene = Object::cast_to<PackedScene>(p_type);
		Node *node;
		if (packed_scene) {
			node = packed_scene->instantiate();
		} else {
			node = Object::cast_to<Node>(p_type->call("new"));
		}

		ERR_FAIL_COND_V_MSG(node == nullptr, nullptr, "Type must return a Node");
		node->set_name(vformat("%s:%d", node->get_class(), child_idx + 1));

		ref = type->value.children.insert(index, UI::create_ui_parented(node, this));
		ref->value->index = index;
		ref->value->deletion = true;
		ref->value->inside = false;
	}

	ERR_FAIL_COND_V_MSG(!ref->value->deletion, nullptr, "Node already added");

	ref->value->deletion = false;
	ref->value->persist = p_persist;
	ref->value->repaint = false;

	if (!ref->value->inside) {
		node->add_child(ref->value->node);
		ref->value->inside = true;
	}

	node->move_child(ref->value->node, child_idx);
	child_idx++;

	return ref->value;
}

Ref<UI> UI::show(const Callable &p_ui_callable) {
	update_callable = p_ui_callable;
	repaint = true;
	
	check_update();

	return this;
}

Ref<UI> UI::prop(const String &p_name, const Variant &p_val) {
	NodePath name = NodePath(p_name);
	if (node->get_indexed(name) != p_val) {
		node->set_indexed(name, p_val);
	}

	return this;
}

Ref<UI> UI::props(const Dictionary &p_props) {
	Array keys = p_props.keys();
	for (int64_t i = keys.size(); i > 0; i--) {
		prop(keys[i - 1], p_props[keys[i - 1]]);
	}

	return this;
}

Ref<UI> UI::motion(const Callable &p_motion_callable) {
	if (node_motion.is_null()) {
		node_motion.instantiate();
		node_motion->node = node;
		node_motion->clear();
	}
	p_motion_callable.call(node_motion);

	return this;
}

Ref<UI> UI::draw(const Callable &p_canvas_item_callable) {
	ERR_FAIL_COND_V_MSG(!Object::cast_to<CanvasItem>(node), this, "Node must inherit CanvasItem");

	if (node_draw.is_null()) {
		CanvasItem *ci = Object::cast_to<CanvasItem>(node);
		node_draw.instantiate();
		node_draw->node = ci;
		node_draw->draw_callable = p_canvas_item_callable;
		ci->connect("draw", callable_mp(node_draw.ptr(), &DrawRef::redraw));
	}

	node_draw->queue_redraw();

	return this;
}

Ref<UI> UI::event(const String &p_signal_name, const Callable &p_target) {
	HashMap<String, SignalInfo>::Iterator signal_info = signals.find(p_signal_name);
	if (!signal_info) {
		signal_info = signals.insert(p_signal_name, SignalInfo());
		signal_info->value.target = Callable();
		signal_info->value.disconnect = true;
	}

	ERR_FAIL_COND_V_MSG(!signal_info->value.disconnect, this, "Signal already connected");

	if (signal_info->value.target.is_null()) {
		node->connect(p_signal_name, p_target);
		signal_info->value.target = p_target;
	}

	if (signal_info->value.target.hash() != p_target.hash()) {
		node->disconnect(p_signal_name, signal_info->value.target);
		node->connect(p_signal_name, p_target);
		signal_info->value.target = p_target;
	}

	signal_info->value.disconnect = false;

	return this;
}

Ref<UI> UI::label(const String &p_text, const Variant &p_key, bool p_persist) {
	
	Ref<UI> label = this->add(get_builtin_class("Label"), p_key, p_persist);
	
	ERR_FAIL_COND_V_MSG(!label.is_valid(), nullptr, "Failed to create a label node");
	
	label->prop("text", p_text);
	
	return label;
}

Ref<UI> UI::button(const String &p_text, const Variant &p_key, bool p_persist) {
	Ref<UI> button = this->add(get_builtin_class("Button"), p_key, p_persist);
	ERR_FAIL_COND_V_MSG(!button.is_valid(), nullptr, "Failed to create a button node");
	button->prop("text", p_text);
	return button;
}

Ref<UI> UI::line_edit(const String &p_text, const Variant &p_key, bool p_persist) {
	Ref<UI> line_edit = this->add(get_builtin_class("LineEdit"), p_key, p_persist);
	ERR_FAIL_COND_V_MSG(!line_edit.is_valid(), nullptr, "Failed to create a line_edit node");
	line_edit->prop("text", p_text);
	return line_edit;
}

Ref<UI> UI::hbox(const Variant &p_key, bool p_persist) {
	Ref<UI> hbox = this->add(get_builtin_class("HBoxContainer"), p_key, p_persist);
	ERR_FAIL_COND_V_MSG(!hbox.is_valid(), nullptr, "Failed to create a hbox node");
	return hbox;
}

Ref<UI> UI::vbox(const Variant &p_key, bool p_persist) {
	Ref<UI> vbox = this->add(get_builtin_class("VBoxContainer"), p_key, p_persist);
	ERR_FAIL_COND_V_MSG(!vbox.is_valid(), nullptr, "Failed to create a vbox node");
	return vbox;
}

Ref<UI> UI::horizontal_scroll(const Callable &p_ui_callable, const Variant &p_key, bool p_persist) {
	Ref<UI> scroll = this->add(get_builtin_class("ScrollContainer"), p_key, p_persist);
	ERR_FAIL_COND_V_MSG(!scroll.is_valid(), nullptr, "Failed to create a scroll node");
	Ref<UI> hbox = scroll->add(get_builtin_class("HBoxContainer"), Variant(), p_persist);
	ERR_FAIL_COND_V_MSG(!hbox.is_valid(), nullptr, "Failed to create a hbox node");
	hbox->show(p_ui_callable);
	return scroll;
}

Ref<UI> UI::vertical_scroll(const Callable &p_ui_callable, const Variant &p_key, bool p_persist) {
	Ref<UI> scroll = this->add(get_builtin_class("ScrollContainer"), p_key, p_persist);
	ERR_FAIL_COND_V_MSG(!scroll.is_valid(), nullptr, "Failed to create a scroll node");
	Ref<UI> vbox = scroll->add(get_builtin_class("VBoxContainer"), Variant(), p_persist);
	ERR_FAIL_COND_V_MSG(!vbox.is_valid(), nullptr, "Failed to create a vbox node");
	vbox->show(p_ui_callable);
	return scroll;
}

Ref<UI> UI::queue_update() {
	if (update_callable.is_null()) {
		UtilityFunctions::push_warning("When using 'queue_update' call 'show' on the respective UI or else it's children won't be updated and will disappear");
	}
	repaint = true;
	return this;
}

Ref<UI> UI::root_queue_update() {
	if (root)
		root->repaint = true;
	else
		repaint = true;
	return this;
}

Node *UI::ref() const {
	return node;
}

Ref<UI> UI::animate_rect(float p_speed) {
	ERR_FAIL_COND_V_MSG(!Object::cast_to<Control>(node), this, "Node must inherit Control");
	Control *control = Object::cast_to<Control>(node);
	if (rect_animation_speed <= 0.0 && p_speed > 0.0) {
		rect_current.size = Vector2(-1.0, -1.0);
	}
	rect_animation_speed = p_speed;
	return this;
}

Ref<UI> UI::theme_variation(const String &p_theme_type) {
	ERR_FAIL_COND_V_MSG(!Object::cast_to<Control>(node), this, "Node must inherit Control");
	Control *control = Object::cast_to<Control>(node);

	control->set_theme_type_variation(p_theme_type);

	return this;
}

Ref<UI> UI::axis_size_flags(bool p_vertical, BitField<Control::SizeFlags> p_flags) {
	ERR_FAIL_COND_V_MSG(!Object::cast_to<Control>(node), this, "Node must inherit Control");
	Control *control = Object::cast_to<Control>(node);

	if (p_vertical) {
		control->set_v_size_flags(p_flags);
	} else {
		control->set_h_size_flags(p_flags);
	}

	return this;
}

Ref<UI> UI::shrink_begin() {
	return axis_size_flags(false, Control::SIZE_SHRINK_BEGIN)->axis_size_flags(true, Control::SIZE_SHRINK_BEGIN);
}

Ref<UI> UI::fill() {
	return axis_size_flags(false, Control::SIZE_FILL)->axis_size_flags(true, Control::SIZE_FILL);
}

Ref<UI> UI::expand() {
	return axis_size_flags(false, Control::SIZE_EXPAND)->axis_size_flags(true, Control::SIZE_EXPAND);
}

Ref<UI> UI::expand_fill() {
	return axis_size_flags(false, Control::SIZE_EXPAND_FILL)->axis_size_flags(true, Control::SIZE_EXPAND_FILL);
}

Ref<UI> UI::shrink_center() {
	return axis_size_flags(false, Control::SIZE_SHRINK_CENTER)->axis_size_flags(true, Control::SIZE_SHRINK_CENTER);
}

Ref<UI> UI::shrink_end() {
	return axis_size_flags(false, Control::SIZE_SHRINK_END)->axis_size_flags(true, Control::SIZE_SHRINK_END);
}

Ref<UI> UI::horizontal_shrink_begin() {
	return axis_size_flags(false, Control::SIZE_SHRINK_BEGIN);
}

Ref<UI> UI::horizontal_fill() {
	return axis_size_flags(false, Control::SIZE_FILL);
}

Ref<UI> UI::horizontal_expand() {
	return axis_size_flags(false, Control::SIZE_EXPAND);
}

Ref<UI> UI::horizontal_expand_fill() {
	return axis_size_flags(false, Control::SIZE_EXPAND_FILL);
}

Ref<UI> UI::horizontal_shrink_center() {
	return axis_size_flags(false, Control::SIZE_SHRINK_CENTER);
}

Ref<UI> UI::horizontal_shrink_end() {
	return axis_size_flags(false, Control::SIZE_SHRINK_END);
}

Ref<UI> UI::vertical_shrink_begin() {
	return axis_size_flags(false, Control::SIZE_SHRINK_BEGIN);
}

Ref<UI> UI::vertical_fill() {
	return axis_size_flags(false, Control::SIZE_FILL);
}

Ref<UI> UI::vertical_expand() {
	return axis_size_flags(false, Control::SIZE_EXPAND);
}

Ref<UI> UI::vertical_expand_fill() {
	return axis_size_flags(false, Control::SIZE_EXPAND_FILL);
}

Ref<UI> UI::vertical_shrink_center() {
	return axis_size_flags(false, Control::SIZE_SHRINK_CENTER);
}

Ref<UI> UI::vertical_shrink_end() {
	return axis_size_flags(false, Control::SIZE_SHRINK_END);
}

Ref<UI> UI::full_rect() {
	ERR_FAIL_COND_V_MSG(!Object::cast_to<Control>(node), this, "Node must inherit Control");
	Control *control = Object::cast_to<Control>(node);

	control->set_anchor_and_offset(SIDE_LEFT, 0.0, 0.0);
	control->set_anchor_and_offset(SIDE_TOP, 0.0, 0.0);
	control->set_anchor_and_offset(SIDE_RIGHT, 1.0, 0.0);
	control->set_anchor_and_offset(SIDE_BOTTOM, 1.0, 0.0);

	return this;
}

Ref<UI> UI::margin(Variant p_unit) {
	ERR_FAIL_COND_V_MSG(!Object::cast_to<Control>(node), this, "Node must inherit Control");
	ERR_FAIL_COND_V_MSG(p_unit.get_type() != Variant::INT && p_unit.get_type() != Variant::FLOAT && p_unit.get_type() != Variant::STRING, this, "Unit must a number or string");
	Control *control = Object::cast_to<Control>(node);
	Variant::Type unit_type = p_unit.get_type();

	Unit unit = unit_type == Variant::STRING ? Unit(((String)p_unit).ptr()) : Unit((float)p_unit);

	ERR_FAIL_COND_V_MSG(unit.type == 0, this, "Invalid unit format");

	if (unit.type == Unit::PIXELS) {
		control->set_anchor_and_offset(SIDE_LEFT, 0.0, unit.value);
		control->set_anchor_and_offset(SIDE_TOP, 0.0, unit.value);
		control->set_anchor_and_offset(SIDE_RIGHT, 1.0, -unit.value);
		control->set_anchor_and_offset(SIDE_BOTTOM, 1.0, -unit.value);
	} else {
		control->set_anchor_and_offset(SIDE_LEFT, unit.value, 0.0);
		control->set_anchor_and_offset(SIDE_TOP, unit.value, 0.0);
		control->set_anchor_and_offset(SIDE_RIGHT, 1.0 - unit.value, 0.0);
		control->set_anchor_and_offset(SIDE_BOTTOM, 1.0 - unit.value, 0.0);
	}

	return this;
}

Ref<UI> UI::horizontal_margin(Variant p_unit) {
	ERR_FAIL_COND_V_MSG(!Object::cast_to<Control>(node), this, "Node must inherit Control");
	ERR_FAIL_COND_V_MSG(p_unit.get_type() != Variant::INT && p_unit.get_type() != Variant::FLOAT && p_unit.get_type() != Variant::STRING, this, "Unit must a number or string");
	Control *control = Object::cast_to<Control>(node);
	Variant::Type unit_type = p_unit.get_type();

	Unit unit = unit_type == Variant::STRING ? Unit(((String)p_unit).ptr()) : Unit((float)p_unit);

	ERR_FAIL_COND_V_MSG(unit.type == 0, this, "Invalid unit format");

	if (unit.type == Unit::PIXELS) {
		control->set_anchor_and_offset(SIDE_LEFT, 0.0, unit.value);
		control->set_anchor_and_offset(SIDE_RIGHT, 1.0, -unit.value);
	} else {
		control->set_anchor_and_offset(SIDE_LEFT, unit.value, 0.0);
		control->set_anchor_and_offset(SIDE_RIGHT, 1.0 - unit.value, 0.0);
	}

	return this;
}

Ref<UI> UI::vertical_margin(Variant p_unit) {
	ERR_FAIL_COND_V_MSG(!Object::cast_to<Control>(node), this, "Node must inherit Control");
	ERR_FAIL_COND_V_MSG(p_unit.get_type() != Variant::INT && p_unit.get_type() != Variant::FLOAT && p_unit.get_type() != Variant::STRING, this, "Unit must a number or string");
	Control *control = Object::cast_to<Control>(node);
	Variant::Type unit_type = p_unit.get_type();

	Unit unit = unit_type == Variant::STRING ? Unit(((String)p_unit).ptr()) : Unit((float)p_unit);

	ERR_FAIL_COND_V_MSG(unit.type == 0, this, "Invalid unit format");

	if (unit.type == Unit::PIXELS) {
		control->set_anchor_and_offset(SIDE_TOP, 0.0, unit.value);
		control->set_anchor_and_offset(SIDE_BOTTOM, 1.0, -unit.value);
	} else {
		control->set_anchor_and_offset(SIDE_TOP, unit.value, 0.0);
		control->set_anchor_and_offset(SIDE_BOTTOM, 1.0 - unit.value, 0.0);
	}

	return this;
}

Ref<UI> UI::left_margin(Variant p_unit) {
	ERR_FAIL_COND_V_MSG(!Object::cast_to<Control>(node), this, "Node must inherit Control");
	ERR_FAIL_COND_V_MSG(p_unit.get_type() != Variant::INT && p_unit.get_type() != Variant::FLOAT && p_unit.get_type() != Variant::STRING, this, "Unit must a number or string");
	Control *control = Object::cast_to<Control>(node);
	Variant::Type unit_type = p_unit.get_type();

	Unit unit = unit_type == Variant::STRING ? Unit(((String)p_unit).ptr()) : Unit((float)p_unit);

	ERR_FAIL_COND_V_MSG(unit.type == 0, this, "Invalid unit format");

	if (unit.type == Unit::PIXELS) {
		control->set_anchor_and_offset(SIDE_LEFT, 0.0, unit.value);
	} else {
		control->set_anchor_and_offset(SIDE_LEFT, unit.value, 0.0);
	}

	return this;
}

Ref<UI> UI::top_margin(Variant p_unit) {
	ERR_FAIL_COND_V_MSG(!Object::cast_to<Control>(node), this, "Node must inherit Control");
	ERR_FAIL_COND_V_MSG(p_unit.get_type() != Variant::INT && p_unit.get_type() != Variant::FLOAT && p_unit.get_type() != Variant::STRING, this, "Unit must a number or string");
	Control *control = Object::cast_to<Control>(node);
	Variant::Type unit_type = p_unit.get_type();

	Unit unit = unit_type == Variant::STRING ? Unit(((String)p_unit).ptr()) : Unit((float)p_unit);

	ERR_FAIL_COND_V_MSG(unit.type == 0, this, "Invalid unit format");

	if (unit.type == Unit::PIXELS) {
		control->set_anchor_and_offset(SIDE_TOP, 0.0, unit.value);
	} else {
		control->set_anchor_and_offset(SIDE_TOP, unit.value, 0.0);
	}

	return this;
}

Ref<UI> UI::right_margin(Variant p_unit) {
	ERR_FAIL_COND_V_MSG(!Object::cast_to<Control>(node), this, "Node must inherit Control");
	ERR_FAIL_COND_V_MSG(p_unit.get_type() != Variant::INT && p_unit.get_type() != Variant::FLOAT && p_unit.get_type() != Variant::STRING, this, "Unit must a number or string");
	Control *control = Object::cast_to<Control>(node);
	Variant::Type unit_type = p_unit.get_type();

	Unit unit = unit_type == Variant::STRING ? Unit(((String)p_unit).ptr()) : Unit((float)p_unit);

	ERR_FAIL_COND_V_MSG(unit.type == 0, this, "Invalid unit format");

	if (unit.type == Unit::PIXELS) {
		control->set_anchor_and_offset(SIDE_RIGHT, 0.0, -unit.value);
	} else {
		control->set_anchor_and_offset(SIDE_RIGHT, 1.0 - unit.value, 0.0);
	}

	return this;
}

Ref<UI> UI::bottom_margin(Variant p_unit) {
	ERR_FAIL_COND_V_MSG(!Object::cast_to<Control>(node), this, "Node must inherit Control");
	ERR_FAIL_COND_V_MSG(p_unit.get_type() != Variant::INT && p_unit.get_type() != Variant::FLOAT && p_unit.get_type() != Variant::STRING, this, "Unit must a number or string");
	Control *control = Object::cast_to<Control>(node);
	Variant::Type unit_type = p_unit.get_type();

	Unit unit = unit_type == Variant::STRING ? Unit(((String)p_unit).ptr()) : Unit((float)p_unit);

	ERR_FAIL_COND_V_MSG(unit.type == 0, this, "Invalid unit format");

	if (unit.type == Unit::PIXELS) {
		control->set_anchor_and_offset(SIDE_BOTTOM, 0.0, -unit.value);
	} else {
		control->set_anchor_and_offset(SIDE_BOTTOM, 1.0 - unit.value, 0.0);
	}

	return this;
}

Ref<UI> UI::create_ui_parented(Node *p_node, const Ref<UI> &p_parent_ui) {
	Ref<UI> ui;
	UI *root = (UI *)p_parent_ui.ptr();
	ui.instantiate();

	if (root) {
		while (root->parent) root = root->parent;
	}

	ui->root = root;
	ui->parent = (UI *)p_parent_ui.ptr();
	ui->node = p_node;

	if (p_parent_ui.is_null()) {
		ERR_FAIL_COND_V_MSG(!p_node->has_method("ui_process"), nullptr, "Target node must have a 'ui_process' function");
		ui->update_callable = p_node->get("ui_process");

		if (p_node->is_inside_tree())
			ui->notification(Node::NOTIFICATION_ENTER_TREE);
	}


	return ui;
}

Ref<UI> UI::create_ui(Node *p_node) {
	return create_ui_parented(p_node, nullptr);
}

void UI::_bind_methods() {
	ClassDB::bind_static_method("UI", D_METHOD("create", "node"), &UI::create_ui, DEFVAL(nullptr));
	ClassDB::bind_static_method("UI", D_METHOD("set_builtin_classes", "classes_dict"), &UI::set_builtin_classes);
	
	ClassDB::bind_method(D_METHOD("clear_children"), &UI::clear_children);
	ClassDB::bind_method(D_METHOD("debug", "enabled"), &UI::debug, DEFVAL(true));
	ClassDB::bind_method(D_METHOD("add", "type", "key", "persist"), &UI::add, DEFVAL(Variant()), DEFVAL(false));
	ClassDB::bind_method(D_METHOD("show", "ui_callable"), &UI::show);
	ClassDB::bind_method(D_METHOD("prop", "name", "value"), &UI::prop);
	ClassDB::bind_method(D_METHOD("props", "props"), &UI::props);
	ClassDB::bind_method(D_METHOD("motion", "motion_callable"), &UI::motion);
	ClassDB::bind_method(D_METHOD("draw", "draw_callable"), &UI::draw);
	ClassDB::bind_method(D_METHOD("event", "signal_name", "target"), &UI::event);
	
	ClassDB::bind_method(D_METHOD("label", "text", "key", "persist"), &UI::label, DEFVAL(Variant()), DEFVAL(false));
	ClassDB::bind_method(D_METHOD("button", "text", "key", "persist"), &UI::button, DEFVAL(Variant()), DEFVAL(false));
	ClassDB::bind_method(D_METHOD("line_edit", "text", "key", "persist"), &UI::line_edit, DEFVAL(Variant()), DEFVAL(false));
	ClassDB::bind_method(D_METHOD("hbox", "key", "persist"), &UI::hbox, DEFVAL(Variant()), DEFVAL(false));
	ClassDB::bind_method(D_METHOD("vbox", "key", "persist"), &UI::vbox, DEFVAL(Variant()), DEFVAL(false));
	ClassDB::bind_method(D_METHOD("horizontal_scroll", "ui_callable", "key", "persist"), &UI::horizontal_scroll, DEFVAL(Variant()), DEFVAL(false));
	ClassDB::bind_method(D_METHOD("vertical_scroll", "ui_callable", "key", "persist"), &UI::vertical_scroll, DEFVAL(Variant()), DEFVAL(false));

	ClassDB::bind_method(D_METHOD("queue_update"), &UI::queue_update);
	ClassDB::bind_method(D_METHOD("root_queue_update"), &UI::root_queue_update);
	
	ClassDB::bind_method(D_METHOD("ref"), &UI::ref);

	ClassDB::bind_method(D_METHOD("animate_rect", "speed"), &UI::animate_rect, DEFVAL(10.0));

	ClassDB::bind_method(D_METHOD("theme_variation", "theme_type"), &UI::theme_variation);
	
	ClassDB::bind_method(D_METHOD("shrink_begin"), &UI::shrink_begin);
	ClassDB::bind_method(D_METHOD("fill"), &UI::fill);
	ClassDB::bind_method(D_METHOD("expand"), &UI::expand);
	ClassDB::bind_method(D_METHOD("expand_fill"), &UI::expand_fill);
	ClassDB::bind_method(D_METHOD("shrink_center"), &UI::shrink_center);
	ClassDB::bind_method(D_METHOD("shrink_end"), &UI::shrink_end);

	ClassDB::bind_method(D_METHOD("horizontal_shrink_begin"), &UI::horizontal_shrink_begin);
	ClassDB::bind_method(D_METHOD("horizontal_fill"), &UI::horizontal_fill);
	ClassDB::bind_method(D_METHOD("horizontal_expand"), &UI::horizontal_expand);
	ClassDB::bind_method(D_METHOD("horizontal_expand_fill"), &UI::horizontal_expand_fill);
	ClassDB::bind_method(D_METHOD("horizontal_shrink_center"), &UI::horizontal_shrink_center);
	ClassDB::bind_method(D_METHOD("horizontal_shrink_end"), &UI::horizontal_shrink_end);

	ClassDB::bind_method(D_METHOD("vertical_shrink_begin"), &UI::vertical_shrink_begin);
	ClassDB::bind_method(D_METHOD("vertical_fill"), &UI::vertical_fill);
	ClassDB::bind_method(D_METHOD("vertical_expand"), &UI::vertical_expand);
	ClassDB::bind_method(D_METHOD("vertical_expand_fill"), &UI::vertical_expand_fill);
	ClassDB::bind_method(D_METHOD("vertical_shrink_center"), &UI::vertical_shrink_center);
	ClassDB::bind_method(D_METHOD("vertical_shrink_end"), &UI::vertical_shrink_end);

	ClassDB::bind_method(D_METHOD("full_rect"), &UI::full_rect);
	ClassDB::bind_method(D_METHOD("margin", "unit"), &UI::margin);
	ClassDB::bind_method(D_METHOD("horizontal_margin", "unit"), &UI::horizontal_margin);
	ClassDB::bind_method(D_METHOD("vertical_margin", "unit"), &UI::vertical_margin);
	ClassDB::bind_method(D_METHOD("left_margin", "unit"), &UI::left_margin);
	ClassDB::bind_method(D_METHOD("top_margin", "unit"), &UI::top_margin);
	ClassDB::bind_method(D_METHOD("right_margin", "unit"), &UI::right_margin);
	ClassDB::bind_method(D_METHOD("bottom_margin", "unit"), &UI::bottom_margin);
}

UI::UI() {
	root = nullptr;
	parent = nullptr;
	index = "";
	node = nullptr;
	signals = HashMap<String, SignalInfo>();
	deletion = false;
	inside = false;
	repaint = true;
	types = UITypeCollection();
	child_idx = 0;

	update_callable = Callable();

	rect_current = Rect2();
	rect_animation_speed = 0.0;
	node_motion = Ref<MotionRef>();
	node_draw = Ref<DrawRef>();

	debug_canvas_item = RID();
	debug_prev_update_elapsed = 1.0;

	Ref<RandomNumberGenerator> rng;
	rng.instantiate();
	rng->randomize();

	debug_color = Color::from_hsv(rng->randf(), 1.0, 1.0, 0.3);
	debug_update_color = debug_color;
}

UI::~UI() {
	node = nullptr;

	for (UITypeCollection::Iterator type = types.begin(); type; ++type) {
		for (UIChildrenCollection::Iterator child = type->value.children.begin(); child; ++child) {
			if (!child->value->inside) {
				child->value->del();
			}
		}
	}

	signals.clear();
	types.clear();

	if (debug_canvas_item.is_valid())
		RenderingServer::get_singleton()->free_rid(debug_canvas_item);
}