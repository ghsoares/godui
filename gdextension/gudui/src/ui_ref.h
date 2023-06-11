#ifndef GUDUI_UI_REF_H
#define GUDUI_UI_REF_H

#include "ui.h"

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/templates/hash_map.hpp>
#include <godot_cpp/variant/callable.hpp>
#include <godot_cpp/variant/string_name.hpp>

namespace godot {

class UIRef : public RefCounted {
	GDCLASS(UI, RefCounted);

	struct SignalInfo {
		Callable target;
		bool disconnect;
	}

	String index;
	Ref<UI> ui;
	Node *node;
	HashMap<StringName, SignalInfo> signals;
	bool deletion;
	bool inside;
	Ref<UI> children;
	Ref<StyleRef> style;
	

protected:
	static void _bind_methods();

public:
	Ref<UIRef> add(Object *p_type, String p_key);
	Ref<UI> style(Callable p_style_callable);
	Ref<UI> queue_update();

	Ref<UIRef> label(String p_text);
	Ref<UIRef> button(String p_text);
	Ref<UIRef> horizontal_scroll(Callable p_children_callable);
	Ref<UIRef> vertical_scroll(Callable p_children_callable);

	UI();
};

}

#endif // GUDUI_UI_REF_H