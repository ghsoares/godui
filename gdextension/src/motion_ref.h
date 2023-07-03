#ifndef GODUI_MOTION_REF_H
#define GODUI_MOTION_REF_H

#include <godot_cpp/variant/variant.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/templates/hash_map.hpp>

namespace godot {

class UI;

class MotionRef : public RefCounted {
	GDCLASS(MotionRef, RefCounted);

	friend class UI;

	struct PropertyKeyframe {
		float time;
		float duration;
		Variant value;
		Variant end_value;
		uint8_t ease_type;
		float ease_strength;

		inline PropertyKeyframe() {}

		inline PropertyKeyframe(
			float p_time, float p_duration, Variant p_value, Variant p_end_value,
			uint8_t p_ease_type, float p_ease_strength
		): time(p_time), duration(p_duration), value(p_value), end_value(p_end_value),
		ease_type(p_ease_type), ease_strength(p_ease_strength) {}
	};

	struct PropertyTrack {
		Vector<PropertyKeyframe> track;
		Variant current_value;
		bool indexed;

		inline PropertyTrack() {
			track = Vector<PropertyKeyframe>();
			current_value = Variant();
			indexed = false;
		}
	};

	struct CallbackKeyframe {
		float time;
		Callable target;

		inline CallbackKeyframe() {}
		inline CallbackKeyframe(float p_time, const Callable &p_target): time(p_time), target(p_target) {}
	};

	Node *node;

	float prev_time;
	float time;
	bool loop_enabled;
	
	float key_time;
	float key_duration;
	float key_scale;
	bool key_parallel;

	HashMap<String, PropertyTrack> property_tracks;
	HashMap<String, PropertyTrack>::Iterator property_track;
	Vector<CallbackKeyframe> callback_track;

protected:
	HashMap<String, PropertyTrack>::Iterator get_property_track(const String &p_name, bool p_indexed);
	void clear();
	void animate();
	// void update_keyframes();

	float eval_time(uint8_t p_ease_type, float p_time, float p_ease_strength);
	void transition_value(const Variant &p_start, const Variant &p_end, Variant &p_out, uint8_t p_ease_type, float p_ease_strength, float p_time, float p_duration);
	void update_substate(bool p_key_parallel, float p_key_time, float p_key_duration);

	static void _bind_methods();
public:
	void reset();

	inline float get_time() const { return key_time; }
	inline float get_duration() const { return key_duration; }

	Ref<MotionRef> loop(bool p_enabled = true);
	Ref<MotionRef> delay(float p_duration);
	Ref<MotionRef> scope(const Callable &p_motion_callable);
	Ref<MotionRef> parallel(const Callable &p_motion_callable);
	Ref<MotionRef> chain(const Callable &p_motion_callable);
	Ref<MotionRef> scale(float p_scale, const Callable &p_motion_callable);
	Ref<MotionRef> prop(const String &p_name, bool p_indexed = false);
	Ref<MotionRef> keyframe(Variant p_value, float p_duration, uint8_t p_ease_mode, float p_ease_strength);
	Ref<MotionRef> callback(const Callable &p_callback_callable);
	Ref<MotionRef> wait(float p_duration);
	Ref<MotionRef> repeat(int p_times, const Callable &p_motion_callable);

	Variant current();
	Variant relative(Variant p_delta);

	Ref<MotionRef> frame(Variant p_value);
	Ref<MotionRef> from_current();
	Ref<MotionRef> constant(Variant p_to_value, float p_duration);
	Ref<MotionRef> linear(Variant p_to_value, float p_duration);

	Ref<MotionRef> ease_in(Variant p_to_value, float p_duration, float p_strength);
	Ref<MotionRef> ease_out(Variant p_to_value, float p_duration, float p_strength);
	Ref<MotionRef> ease_in_out(Variant p_to_value, float p_duration, float p_strength);
	Ref<MotionRef> ease_out_in(Variant p_to_value, float p_duration, float p_strength);

	Ref<MotionRef> elastic_in(Variant p_to_value, float p_duration, float p_strength);
	Ref<MotionRef> elastic_out(Variant p_to_value, float p_duration, float p_strength);
	Ref<MotionRef> elastic_in_out(Variant p_to_value, float p_duration, float p_strength);
	Ref<MotionRef> elastic_out_in(Variant p_to_value, float p_duration, float p_strength);

	Ref<MotionRef> pulse(Variant p_to_value, float p_duration, float p_strength);
	Ref<MotionRef> shake(Variant p_magnitude, float p_duration, float p_strength);

	MotionRef();
};

}

#endif // GODUI_MOTION_REF_H