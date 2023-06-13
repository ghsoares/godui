#include "ui.h"
#include "ui_ref.h"
#include "util.h"

#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/gd_script.hpp>
#include <godot_cpp/classes/gd_script_native_class.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/time.hpp>

using namespace godot;

void UI::ui_update(const Callable &p_ui_callable) {
	UtilityFunctions::print("A");
	for (UIChildrenTypes::Iterator type = children.begin(); type; ++type) {
		for (UIChildren::Iterator child = type->value.begin(); child; ++child) {
			if (child->value->inside) child->value->deletion = true;
		}
	}
	UtilityFunctions::print("B");

	child_idx = 0;
	repaint = false;
	UtilityFunctions::print("K");
	p_ui_callable.call(Ref<UI>(this));

	UtilityFunctions::print("C");

	for (UIChildrenTypes::Iterator type = children.begin(); type; ++type) {
		for (UIChildren::Iterator child = type->value.begin(); child; ++child) {
			if (child->value->deletion) {
				child->value->remove();
			} else {
				child->value->post_update();
			}
		}
	}
	UtilityFunctions::print("D");
}

void UI::idle_update() {
	float delta = parent->get_process_delta_time();

	if (repaint) {
		uint64_t start = Time::get_singleton()->get_ticks_usec();
		UtilityFunctions::print(parent->get("ui_process"));
		ui_update(parent->get("ui_process"));
		uint64_t elapsed = Time::get_singleton()->get_ticks_usec() - start;
		UtilityFunctions::print(vformat("Update time: %.2f ms", ((float)elapsed / 1000.0f)));
	}

	for (UIChildrenTypes::Iterator type = children.begin(); type; ++type) {
		for (UIChildren::Iterator child = type->value.begin(); child; ++child) {
			child->value->idle_update(delta);
		}
	}
}

Ref<UIRef> UI::add(Object *p_type, Variant p_key) {
	ERR_FAIL_COND_V_MSG(
		!p_type->has_method("new"),
		Ref<UIRef>(),
		"must pass a GDScript or a native class as type"
	);

	if (p_key.get_type() != Variant::NIL) {
		p_key = p_key.stringify();
	} else {
		p_key = vformat("__gudui_id:%d", child_idx);
	}

	uint64_t type = p_type->get_instance_id();

	UIChildren children;
	if (this->children.has(type)) {
		children = this->children.get(type);
	} else {
		children = UIChildren();
		this->children.insert(type, children);
	}

	Ref<UIRef> ref;
	if (children.has(p_key)) {
		ref = children.get(p_key);
	} else {
		ref.instantiate();

		ref->index = p_key;
		ref->ui = Ref<UI>(this);
		Node *node = Object::cast_to<Node>(p_type->call("new"));
		ERR_FAIL_COND_V_MSG(node == nullptr, Ref<UIRef>(), "type must return a Node");
		ref->node = node;
		ref->node->set_name(vformat("%s:%d", ref->node->get_class(), child_idx + 1));

		children.insert(p_key, ref);
	}

	if (!ref->inside) {
		parent->add_child(ref->node);
		ref->inside = true;
	}

	parent->move_child(ref->node, child_idx);

	ref->clear();

	child_idx++;

	return ref;
}

Ref<UI> UI::queue_update() {
	root->repaint = true;
	return this;
}

Ref<UI> UI::create_ui(Node *p_parent, Ref<UI> p_root) {
	Ref<UI> ui;
	ui.instantiate();

	ui->root = p_root;
	ui->parent = p_parent;
	ui->repaint = true;
	ui->children = UIChildrenTypes();
	ui->child_idx = 0;

	if (p_root.is_null()) {
		ERR_FAIL_COND_V_MSG(!p_parent->has_method("ui_process"), Ref<UI>(), "target node must have a 'ui_process' function");
		p_parent->get_tree()->connect("process_frame", Callable(ui.ptr(), "idle_update"));
	}

	return ui;
}

void UI::_bind_methods() {
	ClassDB::bind_static_method("UI", D_METHOD("create", "parent", "root"), &UI::create_ui, DEFVAL(Ref<UI>()));
	ClassDB::bind_method(D_METHOD("idle_update"), &UI::idle_update);
	ClassDB::bind_method(D_METHOD("add", "type", "key"), &UI::add, DEFVAL(Variant()));
	ClassDB::bind_method(D_METHOD("queue_update"), &UI::queue_update);
}

UI::UI() {
	root = Ref<UI>();
	parent = nullptr;
	repaint = false;
	children = UIChildrenTypes();
	child_idx = 0;
}