#include "draw_ref.h"
#include "util.h"
#include <godot_cpp/classes/rendering_server.hpp>

using namespace godot;

void DrawRef::redraw() {
	queued_redraw = false;
	if (!draw_callable.is_null())
		draw_callable.call(this);
}

void DrawRef::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_time"), &DrawRef::get_time);
	ClassDB::bind_method(D_METHOD("get_delta"), &DrawRef::get_delta);
	ClassDB::bind_method(D_METHOD("get_node"), &DrawRef::get_node);
	ClassDB::bind_method(D_METHOD("redraw"), &DrawRef::queue_redraw);
	ClassDB::add_property("DrawRef", PropertyInfo(Variant::FLOAT, "time"), "", "get_time");
	ClassDB::add_property("DrawRef", PropertyInfo(Variant::FLOAT, "delta"), "", "get_delta");
	ClassDB::add_property("DrawRef", PropertyInfo(Variant::OBJECT, "node", PROPERTY_HINT_RESOURCE_TYPE, "CanvasItem"), "", "get_node");
}

DrawRef::DrawRef() {
	node = nullptr;

	time = 0.0;
	delta = 0.0;
	draw_callable = Callable();
}




