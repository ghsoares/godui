#ifndef GUDUI_UI_H
#define GUDUI_UI_H

#include "ui_ref.h"
#include "style_ref.h"

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/templates/hash_map.hpp>
#include <godot_cpp/variant/callable.hpp>

namespace godot {

class UI : public RefCounted {
	GDCLASS(UI, RefCounted);

	Ref<UI> root;
	Node *parent;
	bool repaint;
	HashMap<uint64_t, Ref<UIRef>> children;
	uint64_t child_idx;
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

#endif // GUDUI_UI_H