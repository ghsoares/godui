#include "motion_ref.h"
#include "util.h"
#include <godot_cpp/core/math.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

HashMap<String, MotionRef::PropertyTrack>::Iterator MotionRef::get_property_track(const String &p_name, bool p_indexed) {
	HashMap<String, PropertyTrack>::Iterator track = property_tracks.find(p_name);
	if (!track) {
		track = property_tracks.insert(p_name, PropertyTrack());
		track->value.indexed = p_indexed;
		track->value.current_value = p_indexed ? node->get_indexed(NodePath(p_name)) : node->get(StringName(p_name));
	}
	return track;
}

void MotionRef::clear() {
	key_time = 0.0;
	key_duration = 0.0;
	key_parallel = false;
	key_relative = false;
	loop_enabled = false;
	property_tracks.clear();
	callback_track.clear();
	property_track = property_tracks.end();
}

void MotionRef::animate() {
	for (HashMap<String, PropertyTrack>::Iterator track = property_tracks.begin(); track; ++track) {
		uint64_t s = track->value.track.size();
		if (s == 0) continue;

		uint64_t idx = 0;
		for (uint64_t i = 0; i < s; i++) {
			idx = i;
			const PropertyKeyframe &key = track->value.track[i];

			float t = key.time + key.duration;
			if (t > time) break;
		}

		uint64_t prev_idx = idx == 0 ? 0 : idx - 1;

		const PropertyKeyframe &key0 = track->value.track[prev_idx];
		const PropertyKeyframe &key1 = track->value.track[idx];

		Variant val;
		transition_value(key0.value, key1.value, val, key1.ease_type, key1.ease_strength, time - key1.time, key1.duration);
	
		if (track->value.indexed) {
			node->set_indexed(NodePath(track->key), val);
		} else {
			node->set(StringName(track->key), val);
		}
	}

	{
		uint64_t s = callback_track.size();
		if (s != 0) {
			uint64_t idx = 0;
			for (uint64_t i = 0; i < s; i++) {
				const CallbackKeyframe &key = callback_track[i];

				float t = key.time;

				if (prev_time <= t && time > t) {
					key.target.call();
					break;
				}
			}
		}
	}

	if (loop_enabled && time >= key_duration) time -= key_duration;
	prev_time = time;
}

float MotionRef::eval_time(uint8_t p_ease_type, float p_time, float p_strength) {
	if (p_ease_type != 0)
		p_time = p_time < 0.0 ? 0.0 : (p_time > 1.0 ? 1.0 : p_time);
	switch (p_ease_type) {
		case 0: // Constant
			return p_time < 1.0 ? 0.0 : 1.0;
		case 1: // Linear
			return p_time;
		case 2: // Ease in
			return pow(p_time, p_strength);
		case 3: // Ease out
			return 1.0 - pow(1.0 - p_time, p_strength);
		case 4: // Ease in out
			return p_time < 0.5 ? eval_time(2, p_time * 2.0, p_strength) * 0.5 : 0.5 + eval_time(3, (p_time - 0.5) * 2.0, p_strength) * 0.5;
		case 5: // Ease out in
			return p_time < 0.5 ? eval_time(3, p_time * 2.0, p_strength) * 0.5 : 0.5 + eval_time(2, (p_time - 0.5) * 2.0, p_strength) * 0.5;
		case 6: // Elastic in
			return 1.0 - pow(2.0, -10.0 * (1.0 - p_time)) * sin(((1.0 - p_time) * Math_TAU * p_strength - 0.75) * (Math_TAU * 0.333333333)) - 1.0;
		case 7: // Elastic out
			return pow(2.0, -10.0 * p_time) * sin((p_time * Math_TAU * p_strength - 0.75) * (Math_TAU * 0.333333333)) + 1.0;
		case 8: // Elastic in out
			return p_time < 0.5 ? eval_time(6, p_time * 2.0, p_strength) * 0.5 : 0.5 + eval_time(7, (p_time - 0.5) * 2.0, p_strength) * 0.5;
		case 9: // Elastic out in
			return p_time < 0.5 ? eval_time(7, p_time * 2.0, p_strength) * 0.5 : 0.5 + eval_time(6, (p_time - 0.5) * 2.0, p_strength) * 0.5;
		case 10: // Pulse
			return p_time < 0.2 ? eval_time(3, p_time * 5.0, p_strength) : eval_time(4, 1.0 - (p_time - 0.2) * 1.25, p_strength);
		case 11: // Shake
			return sin(p_time * Math_TAU * p_strength) * (1.0 - p_time);
		default: return 0.0;
	}
}

void MotionRef::transition_value(const Variant &p_start, const Variant &p_end, Variant &p_out, uint8_t p_ease_type, float p_ease_strength, float p_time, float p_duration) {
	p_time = p_duration == 0.0 ? (p_time < p_duration ? 0.0 : 1.0) : (p_time / p_duration);
	float t = eval_time(p_ease_type, p_time, p_ease_strength);
	Variant delta, res;
	bool valid;
	Variant::evaluate(Variant::OP_SUBTRACT, p_end, p_start, delta, valid);
	ERR_FAIL_COND_MSG(!valid, "Couldn't evaluate transition between values");
	Variant::evaluate(Variant::OP_MULTIPLY, delta, t, res, valid);
	ERR_FAIL_COND_MSG(!valid, "Couldn't evaluate transition between values");
	Variant::evaluate(Variant::OP_ADD, p_start, res, p_out, valid);
	ERR_FAIL_COND_MSG(!valid, "Couldn't evaluate transition between values");
}

void MotionRef::update_substate(bool p_key_parallel, float p_key_time, float p_key_duration) {
	if (!p_key_parallel) {
		key_time = p_key_time + key_duration;
		key_duration += p_key_duration;
	} else {
		key_time = p_key_time;
		key_duration = key_duration > p_key_duration ? key_duration : p_key_duration;
	}
	key_parallel = p_key_parallel;
}

void MotionRef::reset() {
	time = 0.0;
	prev_time = 0.0;
}

Ref<MotionRef> MotionRef::loop(bool p_enabled) {
	loop_enabled = p_enabled;
	return this;
}

Ref<MotionRef> MotionRef::parallel(const Callable &p_motion_callable) {
	bool key_parallel = this->key_parallel;
	float key_time = this->key_time;
	float key_duration = this->key_duration;

	this->key_parallel = true;
	this->key_duration = 0.0;

	p_motion_callable.call(this);

	update_substate(key_parallel, key_time, key_duration);

	return this;
}

Ref<MotionRef> MotionRef::relative(const Callable &p_motion_callable) {
	bool key_relative = this->key_relative;

	this->key_relative = true;

	p_motion_callable.call(this);

	this->key_relative = key_relative;

	return this;
}

Ref<MotionRef> MotionRef::prop(const String &p_name, const Callable &p_motion_callable) {
	bool key_parallel = this->key_parallel;
	float key_time = this->key_time;
	float key_duration = this->key_duration;
	HashMap<String, PropertyTrack>::Iterator property_track = this->property_track;
	this->property_track = get_property_track(p_name, false);
	this->key_parallel = false;
	this->key_duration = 0.0;

	p_motion_callable.call(this);

	this->property_track = property_track;

	update_substate(key_parallel, key_time, key_duration);

	return this;
}

Ref<MotionRef> MotionRef::keyframe(Variant p_value, float p_duration, uint8_t p_ease_mode, float p_ease_strength) {
	ERR_FAIL_COND_V_MSG(p_duration < 0.0, this, "Duration must be greater or equal 0.0");
	ERR_FAIL_COND_V_MSG(!property_track, this, "Must call 'prop' first");
	PropertyTrack &track = property_track->value;

	if (key_relative) {
		bool valid;
		Variant::evaluate(Variant::OP_ADD, p_value, track.current_value, p_value, valid);
		ERR_FAIL_COND_V_MSG(!valid, this, "Couldn't assign keyframe with different value");
	}
	track.track.append(PropertyKeyframe(key_time, p_duration, p_value, p_ease_mode, p_ease_strength));

	Variant end_val;
	transition_value(track.current_value, p_value, end_val, p_ease_mode, p_ease_strength, 1.0, 1.0);
	track.current_value = end_val;

	if (!key_parallel) {
		key_time += p_duration;
		key_duration += p_duration;
	} else {
		key_duration = key_duration > p_duration ? key_duration : p_duration;
	}

	return this;
}

Ref<MotionRef> MotionRef::callback(const Callable &p_callback_callable) {
	callback_track.append(CallbackKeyframe(key_time, p_callback_callable));
	
	return this;
}

Ref<MotionRef> MotionRef::wait(float p_duration) {
	ERR_FAIL_COND_V_MSG(p_duration < 0.0, this, "Duration must be greater or equal 0.0");

	key_time += p_duration;
	key_duration += p_duration;

	return this;
}

Ref<MotionRef> MotionRef::repeat(int p_times, const Callable &p_motion_callable) {
	bool key_parallel = this->key_parallel;
	float key_time = this->key_time;
	float key_duration = this->key_duration;

	this->key_parallel = false;
	this->key_duration = 0.0;

	for (int i = 0; i < p_times; i++) p_motion_callable.call(this, i);

	update_substate(key_parallel, key_time, key_duration);

	return this;
}

Ref<MotionRef> MotionRef::current() {
	ERR_FAIL_COND_V_MSG(!property_track, this, "Must call 'prop' first");

	return this->keyframe(property_track->value.current_value, 0.0, 0.0, 0.0);
}

Ref<MotionRef> MotionRef::begin() {
	ERR_FAIL_COND_V_MSG(!property_track, this, "Must call 'prop' first");

	return this->keyframe(property_track->value.track[0].value, 0.0, 0.0, 0.0);
}

Ref<MotionRef> MotionRef::frame(Variant p_value) {
	return this->keyframe(p_value, 0.0, 0, 0.0);
}

Ref<MotionRef> MotionRef::constant(Variant p_value, float p_duration) {
	return this->keyframe(p_value, p_duration, 0, 0.0);
}

Ref<MotionRef> MotionRef::linear(Variant p_to_value, float p_duration) {
	return this->keyframe(p_to_value, p_duration, 1, 0.0);
}

Ref<MotionRef> MotionRef::ease_in(Variant p_to_value, float p_duration, float p_strength) {
	return this->keyframe(p_to_value, p_duration, 2, p_strength);
}

Ref<MotionRef> MotionRef::ease_out(Variant p_to_value, float p_duration, float p_strength) {
	return this->keyframe(p_to_value, p_duration, 3, p_strength);
}

Ref<MotionRef> MotionRef::ease_in_out(Variant p_to_value, float p_duration, float p_strength) {
	return this->keyframe(p_to_value, p_duration, 4, p_strength);
}

Ref<MotionRef> MotionRef::ease_out_in(Variant p_to_value, float p_duration, float p_strength) {
	return this->keyframe(p_to_value, p_duration, 5, p_strength);
}

Ref<MotionRef> MotionRef::elastic_in(Variant p_to_value, float p_duration, float p_strength) {
	return this->keyframe(p_to_value, p_duration, 6, p_strength);
}

Ref<MotionRef> MotionRef::elastic_out(Variant p_to_value, float p_duration, float p_strength) {
	return this->keyframe(p_to_value, p_duration, 7, p_strength);
}

Ref<MotionRef> MotionRef::elastic_in_out(Variant p_to_value, float p_duration, float p_strength) {
	return this->keyframe(p_to_value, p_duration, 8, p_strength);
}

Ref<MotionRef> MotionRef::elastic_out_in(Variant p_to_value, float p_duration, float p_strength) {
	return this->keyframe(p_to_value, p_duration, 9, p_strength);
}

Ref<MotionRef> MotionRef::pulse(Variant p_to_value, float p_duration, float p_strength) {
	return this->keyframe(p_to_value, p_duration, 10, p_strength);
}

Ref<MotionRef> MotionRef::shake(Variant p_magnitude, float p_duration, float p_strength) {
	return this->keyframe(p_magnitude, p_duration, 11, p_strength);
}

void MotionRef::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_time"), &MotionRef::get_time);
	ClassDB::bind_method(D_METHOD("get_duration"), &MotionRef::get_duration);

	ClassDB::bind_method(D_METHOD("reset"), &MotionRef::reset);
	ClassDB::bind_method(D_METHOD("loop", "enabled"), &MotionRef::loop);
	ClassDB::bind_method(D_METHOD("parallel", "motion_callable"), &MotionRef::parallel);
	ClassDB::bind_method(D_METHOD("relative", "motion_callable"), &MotionRef::relative);
	ClassDB::bind_method(D_METHOD("prop", "name", "motion_callable"), &MotionRef::prop);
	// ClassDB::bind_method(D_METHOD("keyframe", "value", "duration", "ease_mode", "ease_strength"), &MotionRef::keyframe);
	ClassDB::bind_method(D_METHOD("callback", "callback_callable"), &MotionRef::callback);
	ClassDB::bind_method(D_METHOD("wait", "duration"), &MotionRef::wait);
	ClassDB::bind_method(D_METHOD("repeat", "times", "motion_callable"), &MotionRef::repeat);

	ClassDB::bind_method(D_METHOD("current"), &MotionRef::current);
	ClassDB::bind_method(D_METHOD("begin"), &MotionRef::begin);
	ClassDB::bind_method(D_METHOD("frame", "value"), &MotionRef::frame);
	ClassDB::bind_method(D_METHOD("constant", "to_value", "duration"), &MotionRef::constant);
	ClassDB::bind_method(D_METHOD("linear", "to_value", "duration"), &MotionRef::linear);
	ClassDB::bind_method(D_METHOD("ease_in", "to_value", "duration", "strength"), &MotionRef::ease_in, DEFVAL(3.0));
	ClassDB::bind_method(D_METHOD("ease_out", "to_value", "duration", "strength"), &MotionRef::ease_out, DEFVAL(3.0));
	ClassDB::bind_method(D_METHOD("ease_in_out", "to_value", "duration", "strength"), &MotionRef::ease_in_out, DEFVAL(3.0));
	ClassDB::bind_method(D_METHOD("ease_out_in", "to_value", "duration", "strength"), &MotionRef::ease_out_in, DEFVAL(3.0));
	ClassDB::bind_method(D_METHOD("elastic_in", "to_value", "duration", "strength"), &MotionRef::elastic_in, DEFVAL(3.0));
	ClassDB::bind_method(D_METHOD("elastic_out", "to_value", "duration", "strength"), &MotionRef::elastic_out, DEFVAL(3.0));
	ClassDB::bind_method(D_METHOD("elastic_in_out", "to_value", "duration", "strength"), &MotionRef::elastic_in_out, DEFVAL(3.0));
	ClassDB::bind_method(D_METHOD("elastic_out_in", "to_value", "duration", "strength"), &MotionRef::elastic_out_in, DEFVAL(3.0));
	ClassDB::bind_method(D_METHOD("pulse", "to_value", "duration", "strength"), &MotionRef::pulse, DEFVAL(3.0));
	ClassDB::bind_method(D_METHOD("shake", "magnitude", "duration", "strength"), &MotionRef::shake, DEFVAL(3.0));
}

MotionRef::MotionRef() {
	node = nullptr;
	prev_time = 0.0;
	time = 0.0;
	loop_enabled = false;
	key_time = 0.0;
	key_duration = 0.0;
	key_parallel = 0.0;
	property_tracks = HashMap<String, PropertyTrack>();
	property_track = property_tracks.end();
	callback_track = Vector<CallbackKeyframe>();
}