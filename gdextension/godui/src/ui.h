#ifndef GODUI_UI_H
#define GODUI_UI_H

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/templates/hash_map.hpp>
#include <godot_cpp/variant/callable.hpp>

namespace godot {

class UI : public RefCounted {
	GDCLASS(UI, RefCounted);

	struct SignalInfo {
		Callable target;
		bool disconnect;
	};

	struct ChildTypeKey {
		uint8_t type;
		uint64_t value;

		inline ChildTypeKey(): type(0), value(0) {}
		inline ChildTypeKey(uint8_t p_type, uint64_t p_value): type(p_type), value(p_value) {}
		
		inline static ChildTypeKey from_class(Object *p_class) {
			return ChildTypeKey(0, p_class->get_instance_id());
		}
		inline static ChildTypeKey from_callable(const Callable &p_callable) {
			return ChildTypeKey(1, *(uint64_t *)((int64_t *)p_callable.hash()));
		}

		inline bool operator==(const ChildTypeKey &p_other) const {
			return this->type == p_other.type && this->value == p_other.value;
		}
		inline bool operator!=(const ChildTypeKey &p_other) const {
			return !(*this == p_other);
		}

		inline ChildTypeKey &operator=(const ChildTypeKey &p_other) {
			this->type = p_other.type;
			this->value = p_other.value;
			return *this;
		}

		static uint32_t hash(const ChildTypeKey &p_key) {
			return HashMapHasherDefault::hash(p_key.type) ^ HashMapHasherDefault::hash(p_key.value);
		}
	};

	using UIChildren = HashMap<String, Ref<UI>>;
	using UIChildrenTypes = HashMap<ChildTypeKey, UIChildren, ChildTypeKey>;

	Ref<UI> root;
	Ref<UI> parent;
	String index;
	Node *node;
	HashMap<String, SignalInfo> signals;
	bool deletion;
	bool inside;
	bool repaint;
	UIChildrenTypes children;
	uint64_t child_idx;

protected:
	static void _bind_methods();

	void clear();
	void idle_update(float p_delta);
	void ui_update(const Callable &p_ui_callable);
	void post_update();
	void remove();

	bool extract_anchor_unit(const char *p_unit, float &p_anchor_pos, float &p_anchor_off);
public:
	void frame_update();

	Ref<UI> add(Variant p_type, const Variant &p_key);

	Ref<UI> prop(const NodePath &p_name, const Variant &p_val);
	Ref<UI> props(const Dictionary &p_props);

	Ref<UI> event(const String &p_signal_name, const Callable &p_target);

	Ref<UI> show(const Callable &p_ui_callable);
	Ref<UI> queue_update();

	Node *ref();

	Ref<UI> theme_variation(const String &p_theme_type);

	Ref<UI> axis_size_flags(bool p_vertical, BitField<Control::SizeFlags> p_flags);
	
	Ref<UI> shrink_begin();
	Ref<UI> fill();
	Ref<UI> expand();
	Ref<UI> expand_fill();
	Ref<UI> shrink_center();
	Ref<UI> shrink_end();

	Ref<UI> horizontal_shrink_begin();
	Ref<UI> horizontal_fill();
	Ref<UI> horizontal_expand();
	Ref<UI> horizontal_expand_fill();
	Ref<UI> horizontal_shrink_center();
	Ref<UI> horizontal_shrink_end();

	Ref<UI> vertical_shrink_begin();
	Ref<UI> vertical_fill();
	Ref<UI> vertical_expand();
	Ref<UI> vertical_expand_fill();
	Ref<UI> vertical_shrink_center();
	Ref<UI> vertical_shrink_end();

	Ref<UI> full_rect();
	Ref<UI> margin(Variant unit);
	Ref<UI> horizontal_margin(Variant unit);
	Ref<UI> vertical_margin(Variant unit);
	Ref<UI> left_margin(Variant unit);
	Ref<UI> top_margin(Variant unit);
	Ref<UI> right_margin(Variant unit);
	Ref<UI> bottom_margin(Variant unit);

	static Ref<UI> create_ui(Node *p_node, const Ref<UI> &p_parent_ui);

	UI();
};

}

#endif // GODUI_UI_H