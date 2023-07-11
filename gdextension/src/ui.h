#ifndef GODUI_UI_H
#define GODUI_UI_H

#include "motion_ref.h"
#include "draw_ref.h"

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/font.hpp>
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

	static HashMap<String, Object *> builtin_scripts;
	static bool builtin_scripts_initialized;

	using UITypeCollection = HashMap<uint64_t, UINodeCollection>;
	using UIChildrenCollection = HashMap<String, Ref<UI>>;

	UI *root;
	UI *parent;
	String index;
	Node *node;
	HashMap<String, SignalInfo> signals;
	bool persist;
	bool deletion;
	bool inside;
	bool repaint;
	UITypeCollection types;
	uint64_t child_idx;

	Callable update_callable;

	Rect2 rect_current;
	float rect_animation_speed;
	Ref<MotionRef> node_motion;
	Ref<DrawRef> node_draw;

	RID debug_canvas_item;
	Ref<Font> debug_font;
	float debug_prev_update_elapsed;
	Color debug_color;
	Color debug_update_color;

protected:
	static void _bind_methods();

	void _notification(int p_what);

	void check_update();
	void pre_update();
	void ui_process();
	void post_update();
	
	void remove();
	void del();
	void idle_update(float p_delta);
	void draw_update(float p_delta);

	void initialize_builtin_classes();
	Object *get_builtin_class(const String &p_class);

	bool extract_anchor_unit(const char *p_unit, float &p_anchor_pos, float &p_anchor_off);

public:
	void before_draw();
	void clear_children();

	Ref<UI> debug(bool p_enabled = true);

	Ref<UI> add(Object *p_type, const Variant &p_key = Variant(), bool p_persist = false);
	Ref<UI> show(const Callable &p_ui_callable);

	Ref<UI> prop(const String &p_name, const Variant &p_val);
	Ref<UI> props(const Dictionary &p_props);
	Ref<UI> motion(const Callable &p_motion_callable);
	Ref<UI> draw(const Callable &p_canvas_item_callable);
	Ref<UI> event(const String &p_signal_name, const Callable &p_target);

	Ref<UI> label(const String &p_text, const Variant &p_key = Variant(), bool p_persist = false);
	Ref<UI> button(const String &p_text, const Variant &p_key = Variant(), bool p_persist = false);
	Ref<UI> line_edit(const String &p_input_text, const Variant &p_key = Variant(), bool p_persist = false);
	Ref<UI> hbox(const Variant &p_key = Variant(), bool p_persist = false);
	Ref<UI> vbox(const Variant &p_key = Variant(), bool p_persist = false);
	Ref<UI> horizontal_scroll(const Callable &p_ui_callable, const Variant &p_key = Variant(), bool p_persist = false);
	Ref<UI> vertical_scroll(const Callable &p_ui_callable, const Variant &p_key = Variant(), bool p_persist = false);

	Ref<UI> queue_update();
	Ref<UI> root_queue_update();

	Node *ref() const;

	Ref<UI> animate_rect(float p_speed = 10.0);

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

	static Ref<UI> create_ui_parented(Node *p_node, const Ref<UI> &p_parent_ui);
	static Ref<UI> create_ui(Node *p_node);

	UI();
	~UI();
};

}

#endif // GODUI_UI_H