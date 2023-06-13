#ifndef GODUI_UI_REF_H
#define GODUI_UI_REF_H

#include "ui.h"

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/templates/hash_map.hpp>
#include <godot_cpp/variant/callable.hpp>
#include <godot_cpp/variant/string_name.hpp>
#include <godot_cpp/variant/dictionary.hpp>

namespace godot {

class UIRef : public RefCounted {
	GDCLASS(UIRef, RefCounted);

	friend class UI;

	String index;
	Ref<UI> ui;
	Node *node;
	bool deletion;
	bool inside;
	Ref<UI> children;

protected:
	static void _bind_methods();

	void clear();
	void idle_update(float p_delta);
	void post_update();
	void remove();

public:
	Ref<UIRef> prop(StringName p_name, Variant p_val);
	Ref<UIRef> props(Dictionary p_props);
	Ref<UIRef> show(const Callable &p_ui_callable);
	Node *ref();

	UIRef();
};

}

#endif // GODUI_UI_REF_H