#ifndef GODUI_UI_H
#define GODUI_UI_H

#include "motion_ref.h"

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

	struct UINodeCollection {
		uint64_t idx;
		HashMap<String, Ref<UI>> children;

		inline UINodeCollection() {
			idx = 0;
			children = HashMap<String, Ref<UI>>();
		}
	};

	using UITypeCollection = HashMap<uint64_t, UINodeCollection>;
	using UIChildrenCollection = HashMap<String, Ref<UI>>;

	Ref<UI> root;
	Ref<UI> parent;
	String index;
	Node *node;
	HashMap<String, SignalInfo> signals;
	bool persist;
	bool deletion;
	bool inside;
	bool repaint;
	UITypeCollection types;
	uint64_t child_idx;

	Rect2 rect_current;
	float rect_animation_speed;
	Ref<MotionRef> node_motion;

protected:
	static void _bind_methods();

	void pre_update();
	void ui_update(const Callable &p_ui_callable);
	void post_update();
	void remove();
	void del();
	void idle_update(float p_delta);
	void draw_update(float p_delta);

	bool extract_anchor_unit(const char *p_unit, float &p_anchor_pos, float &p_anchor_off);
public:
	void process_frame();
	void before_draw();

	Ref<UI> add(Object *p_type, const Variant &p_key, bool p_persist);

	Ref<UI> prop(const NodePath &p_name, const Variant &p_val);
	Ref<UI> props(const Dictionary &p_props);
	Ref<UI> motion(const Callable &p_motion_callable);
	Ref<UI> event(const String &p_signal_name, const Callable &p_target);

	Ref<UI> scope(const Callable &p_ui_callable);
	Ref<UI> queue_update();

	Node *ref();

	Ref<UI> animate_rect(float p_speed);

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
	~UI();
};

}

#endif // GODUI_UI_H