#ifndef GODUI_MOTION_REF_H
#define GODUI_MOTION_REF_H

#include <godot_cpp/variant/variant.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/templates/hash_map.hpp>

namespace godot {

class MotionRef : public RefCounted {
	GDCLASS(MotionRef, RefCounted);

	struct PropertyKeyframe {
		float time;
		float duration;
		Variant value;
		int ease_type;
		float ease_strength;
	};

	Node *node;

	float prev_time;
	float time;
	bool loop;
	
	float key_time;
	float key_duration;
	bool key_parallel;

	HashMap<String, Vector<PropertyKeyframe>> property_tracks;

protected:
	Vector<PropertyKeyframe> get_property_track(String p_name);
	void clear();
	void animate();
	void update_keyframes();

	static void _bind_methods();

public:
	void reset();
	Ref<MotionRef> loop(bool p_enabled);
	Ref<MotionRef> parallel(Callable p_motion_callable);
	Ref<MotionRef> prop(String p_name, Callable p_motion_callable);
	Ref<MotionRef> keyframe(Variant p_value, float p_duration, int p_ease_mode, float p_ease_strength);
	Ref<MotionRef> wait(float p_duration);

	MotionRef();
};

}

#endif // GODUI_MOTION_REF_H