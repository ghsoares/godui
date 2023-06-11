#ifndef FRAMERATE_ANIMATION_PLAYER
#define FRAMERATE_ANIMATION_PLAYER

#include <godot_cpp/classes/animation_player.hpp>

namespace godot {

class FramerateAnimationPlayer : public AnimationPlayer {
	GDCLASS(FramerateAnimationPlayer, AnimationPlayer);
	
	real_t fps = 0.0;
	real_t fc = 0.0;

protected:
	void _notification(int p_what);
	static void _bind_methods();

public:
	void set_fps(real_t p_fps);
	real_t get_fps() const;

	FramerateAnimationPlayer();
};

}

#endif // FRAMERATE_ANIMATION_PLAYER