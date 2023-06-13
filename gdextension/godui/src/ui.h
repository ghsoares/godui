#ifndef GODUI_UI_H
#define GODUI_UI_H

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/templates/hash_map.hpp>
#include <godot_cpp/variant/callable.hpp>

namespace godot {

class UIRef;

using UIChildren = HashMap<String, Ref<UIRef>>;
using UIChildrenTypes = HashMap<uint64_t, UIChildren>;

class UI : public RefCounted {
	GDCLASS(UI, RefCounted);

	friend class UIRef;

	Ref<UI> root;
	Node *parent;
	bool repaint;
	UIChildrenTypes children;
	uint64_t child_idx;

protected:
	static void _bind_methods();

	void ui_update(const Callable &p_ui_callable);
public:
	void idle_update();
	Ref<UIRef> add(Object *p_type, Variant p_key);
	Ref<UI> queue_update();

	static Ref<UI> create_ui(Node *p_parent, Ref<UI> p_root);

	UI();
};

}

#endif // GODUI_UI_H