#ifndef COLLISION_SHAPE_SPATIAL_2D_H
#define COLLISION_SHAPE_SPATIAL_2D_H

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/shape2d.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/variant/rect2.hpp>
#include <godot_cpp/variant/color.hpp>
#include <godot_cpp/variant/string_name.hpp>

namespace godot {

class CollisionObjectSpatial2D;

class CollisionShapeSpatial2D : public Node3D {
	GDCLASS(CollisionShapeSpatial2D, Node3D);
	Ref<Shape2D> shape = Ref<Shape2D>();
	Rect2 rect = Rect2(-Vector2(10, 10), Vector2(20, 20));
	uint32_t owner_id = 0;
	CollisionObjectSpatial2D *parent = nullptr;
	bool disabled = false;
	bool one_way_collision = false;
	real_t one_way_collision_margin = 1.0;
	Color debug_color;

	void _update_in_shape_owner(bool p_xform_only = false);
	Color _get_default_debug_color() const;

protected:
	void _notification(int p_what);
	bool _property_can_revert(const StringName &p_name) const;
	bool _property_get_revert(const StringName &p_name, Variant &r_property) const;
	void _validate_property(PropertyInfo &p_property) const;
	static void _bind_methods();

public:
	void _shape_changed();

	Transform2D get_transform_2d() const;

	void set_shape(const Ref<Shape2D> &p_shape);
	Ref<Shape2D> get_shape() const;

	void set_disabled(bool p_disabled);
	bool is_disabled() const;

	void set_one_way_collision(bool p_enable);
	bool is_one_way_collision_enabled() const;

	void set_one_way_collision_margin(real_t p_margin);
	real_t get_one_way_collision_margin() const;

	void set_debug_color(const Color &p_color);
	Color get_debug_color() const;

	PackedStringArray _get_configuration_warnings() const override;

	CollisionShapeSpatial2D();
};

}

#endif // COLLISION_SHAPE_SPATIAL_2D_H