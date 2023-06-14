#include "ui.h"
#include "util.h"
#include "unit.h"

#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/gd_script.hpp>
#include <godot_cpp/classes/gd_script_native_class.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/time.hpp>

using namespace godot;

void UI::frame_update() {
	if (repaint) {
		clear();
		ui_update(node->get("ui_process"));
		post_update();
	}

	idle_update(node->get_process_delta_time());
}

void UI::clear() {
	for (UIChildrenTypes::Iterator type = children.begin(); type; ++type) {
		for (UIChildren::Iterator child = type->value.begin(); child; ++child) {
			if (child->value->inside) child->value->deletion = true;
		}
	}

	for (HashMap<String, SignalInfo>::Iterator signal = signals.begin(); signal; ++signal) {
		if (!signal->value.target.is_null()) {
			signal->value.disconnect = true;
		}
	}

	node->set_block_signals(true);

	child_idx = 0;
	repaint = false;
	deletion = false;
}

void UI::idle_update(float p_delta) {
	for (UIChildrenTypes::Iterator type = children.begin(); type; ++type) {
		for (UIChildren::Iterator child = type->value.begin(); child; ++child) {
			child->value->idle_update(p_delta);
		}
	}
}

void UI::ui_update(const Callable &p_ui_callable) {
	p_ui_callable.call(this);
}

void UI::post_update() {
	for (UIChildrenTypes::Iterator type = children.begin(); type; ++type) {
		for (UIChildren::Iterator child = type->value.begin(); child; ++child) {
			if (child->value->deletion) {
				child->value->remove();
			} else {
				child->value->post_update();
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
}

void UI::remove() {
	parent->node->remove_child(node);

	for (HashMap<String, SignalInfo>::Iterator signal = signals.begin(); signal; ++signal) {
		if (!signal->value.target.is_null()) {
			node->disconnect(signal->key, signal->value.target);
		}
	}

	signals.clear();

	inside = false;
	deletion = false;
}

Ref<UI> UI::add(Variant p_type, const Variant &p_key) {
	String index;

	if (p_key.get_type() != Variant::NIL) {
		index = p_key.stringify();
	} else {
		index = vformat("__gudui_id:%d", child_idx);
	}

	ChildTypeKey type_key;

	switch (p_type.get_type()) {
		case Variant::OBJECT: {
			ERR_FAIL_COND_V_MSG(
				!((Object *)p_type)->has_method("new"),
				this,
				"Type must be a native class or a script"
			);
			type_key = ChildTypeKey::from_class(p_type);
		} break;
		case Variant::CALLABLE: {
			type_key = ChildTypeKey::from_callable(p_type);
		} break;
		default: ERR_FAIL_V_MSG(this, "Invalid argument for type");
	}

	UIChildrenTypes::Iterator children = this->children.find(type_key);
	if (!children) {
		children = this->children.insert(type_key, UIChildren());
	}

	UIChildren::Iterator ref = children->value.find(index);
	if (!ref) {
		Node *node = nullptr;
		switch (p_type.get_type()) {
			case Variant::OBJECT: {
				node = Object::cast_to<Node>(((Object *)p_type)->call("new"));
			} break;
			case Variant::CALLABLE: {
				Variant ret = ((Callable)p_type).call();
				ERR_FAIL_COND_V_MSG(ret.get_type() != Variant::OBJECT || !Object::cast_to<Node>(ret), this, "Function doesn't return a node");
				node = Object::cast_to<Node>(ret);
			} break;
		}
		ERR_FAIL_COND_V_MSG(node == nullptr, Ref<UI>(), "Type must return a Node");
		node->set_name(vformat("%s:%d", node->get_class(), child_idx + 1));

		ref = children->value.insert(index, UI::create_ui(node, this));
		ref->value->index = index;
	}

	if (!ref->value->inside) {
		node->add_child(ref->value->node);
		ref->value->inside = true;
	}

	node->move_child(ref->value->node, child_idx);

	ref->value->clear();

	child_idx++;

	return ref->value;
}

Ref<UI> UI::prop(const NodePath &p_name, const Variant &p_val) {
	if (node->get_indexed(p_name) != p_val) {
		node->set_indexed(p_name, p_val);
	}

	return this;
}

Ref<UI> UI::props(const Dictionary &p_props) {
	Array keys = p_props.keys();
	for (int64_t i = keys.size() - 1; i >= 0; i--) {
		NodePath p = NodePath((String)keys[i]);
		prop(p, p_props[keys[i]]);
	}

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

Ref<UI> UI::show(const Callable &p_ui_callable) {
	ui_update(p_ui_callable);

	return this;
}

Ref<UI> UI::queue_update() {
	root->repaint = true;
	return this;
}

Node *UI::ref() {
	return node;
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

Ref<UI> UI::create_ui(Node *p_node, const Ref<UI> &p_parent_ui) {
	Ref<UI> ui;
	Ref<UI> root = p_parent_ui;
	ui.instantiate();

	if (root.is_valid()) {
		while (root->parent.is_valid()) {
			root = root->parent;
		}
	}

	ui->root = root.is_valid() ? root : ui;
	ui->parent = p_parent_ui;
	ui->node = p_node;

	if (p_parent_ui.is_null()) {
		ERR_FAIL_COND_V_MSG(!p_node->has_method("ui_process"), Ref<UI>(), "Target node must have a 'ui_process' function");
		p_node->get_tree()->connect("process_frame", Callable(ui.ptr(), "frame_update"));
	}

	return ui;
}

void UI::_bind_methods() {
	ClassDB::bind_static_method("UI", D_METHOD("create", "Node", "parent_ui"), &UI::create_ui, DEFVAL(Ref<UI>()));
	
	ClassDB::bind_method(D_METHOD("frame_update"), &UI::frame_update);
	ClassDB::bind_method(D_METHOD("add", "type", "key"), &UI::add, DEFVAL(Variant()));
	ClassDB::bind_method(D_METHOD("prop", "name", "value"), &UI::prop);
	ClassDB::bind_method(D_METHOD("props", "props"), &UI::props);
	ClassDB::bind_method(D_METHOD("event", "signal_name", "target"), &UI::event);
	ClassDB::bind_method(D_METHOD("show", "ui_callable"), &UI::show);
	ClassDB::bind_method(D_METHOD("queue_update"), &UI::queue_update);
	ClassDB::bind_method(D_METHOD("ref"), &UI::ref);

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
	root = Ref<UI>();
	parent = Ref<UI>();
	index = "";
	node = nullptr;
	signals = HashMap<String, SignalInfo>();
	deletion = false;
	inside = false;
	repaint = true;
	children = UIChildrenTypes();
	child_idx = 0;
}