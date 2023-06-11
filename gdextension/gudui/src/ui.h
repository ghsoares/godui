#ifndef GUDUI_UI_TYPES_H
#define GUDUI_UI_TYPES_H

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/templates/hash_map.hpp>

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

	UI();
};

}

#endif // GUDUI_UI_TYPES_H