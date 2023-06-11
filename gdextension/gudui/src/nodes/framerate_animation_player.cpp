#include "framerate_animation_player.h"

#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void FramerateAnimationPlayer::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			set_process_callback(ANIMATION_PROCESS_MANUAL);
			set_physics_process(true);
		} break;
		case NOTIFICATION_PHYSICS_PROCESS: {
			if (fps <= 0.0) {
				if (is_playing()) advance(get_physics_process_delta_time());
			} else {
				fc += fps * get_speed_scale() * get_physics_process_delta_time();

				while (fc >= 1.0) {
					if (is_playing()) advance(1.0 / fps / get_speed_scale());
					fc -= 1.0;
				}
			}
		} break;
	}
}

void FramerateAnimationPlayer::set_fps(real_t p_fps) {
	fps = p_fps;
}

real_t FramerateAnimationPlayer::get_fps() const {
	return fps;
}

void FramerateAnimationPlayer::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_fps", "fps"), &FramerateAnimationPlayer::set_fps);
	ClassDB::bind_method(D_METHOD("get_fps"), &FramerateAnimationPlayer::get_fps);

	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "fps", PROPERTY_HINT_RANGE, "0,60,0.1,or_greater"), "set_fps", "get_fps");
}

FramerateAnimationPlayer::FramerateAnimationPlayer() {
	
}