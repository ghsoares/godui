#ifndef GODUI_DRAW_REF_H
#define GODUI_DRAW_REF_H

#include <godot_cpp/variant/rid.hpp>
#include <godot_cpp/variant/callable.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/classes/canvas_item.hpp>

namespace godot {

class UI;

class DrawRef : public RefCounted {
	GDCLASS(DrawRef, RefCounted);

	friend class UI;

	CanvasItem *node;

	float time;
	float delta;
	bool queued_redraw;

	Callable draw_callable;

protected:
	static void _bind_methods();
	
	void redraw();

public:
	inline float get_time() const { return this->time; }
	inline float get_delta() const { return this->delta; }
	inline CanvasItem * get_node() const { return this->node; }
	
	inline void queue_redraw() { queued_redraw = true; }

	DrawRef();
};

}

#endif // GODUI_DRAW_REF_H