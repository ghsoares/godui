#include "ui_ref.h"

using namespace godot;

void UIRef::clear() {
	deletion = false;
}

void UIRef::idle_update(float p_delta) {
	if (children.is_valid()) {
		children->idle_update();
	}
}

void UIRef::post_update() {
	
}

void UIRef::remove() {
	ui->parent->remove_child(node);

	inside = false;
	deletion = false;
}

Ref<UIRef> UIRef::prop(StringName p_name, Variant p_val) {
	if (node->get(p_name) != p_val) {
		node->set(p_name, p_val);
	}

	return this;
}

Ref<UIRef> UIRef::props(Dictionary p_props) {
	Array keys = p_props.keys();
	for (int64_t i = keys.size() - 1; i >= 0; i--) {
		prop(keys[i], p_props[keys[i]]);
	}

	return this;
}

Ref<UIRef> UIRef::show(const Callable &p_ui_callable) {
	if (children.is_null()) {
		children = UI::create_ui(node, ui->root);
	}

	children->ui_update(p_ui_callable);

	return this;
}

Node *UIRef::ref() {
	return node;
}

void UIRef::_bind_methods() {
	ClassDB::bind_method(D_METHOD("prop", "name", "val"), &UIRef::prop);
	ClassDB::bind_method(D_METHOD("props", "props"), &UIRef::props);
	ClassDB::bind_method(D_METHOD("show", "ui_callable"), &UIRef::show);
	ClassDB::bind_method(D_METHOD("ref"), &UIRef::ref);
}

UIRef::UIRef() {
	index = "";
	ui = Ref<UI>();
	node = nullptr;
	deletion = false;
	inside = false;
	children = Ref<UI>();
}
