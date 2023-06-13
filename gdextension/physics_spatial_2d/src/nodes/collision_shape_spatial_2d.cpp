#include "collision_shape_spatial_2d.h"
#include "collision_object_spatial_2d.h"
#include "../util/object.hpp"

#include <godot_cpp/classes/area2d.hpp>
#include <godot_cpp/classes/concave_polygon_shape2d.hpp>
#include <godot_cpp/classes/convex_polygon_shape2d.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void CollisionShapeSpatial2D::_shape_changed() {
	update_gizmos();
}

void CollisionShapeSpatial2D::_update_in_shape_owner(bool p_xform_only) {
	parent->shape_owner_set_transform(owner_id, get_transform_2d());
	if (p_xform_only) {
		return;
	}
	parent->shape_owner_set_disabled(owner_id, disabled);
	parent->shape_owner_set_one_way_collision(owner_id, one_way_collision);
	parent->shape_owner_set_one_way_collision_margin(owner_id, one_way_collision_margin);
}

Color CollisionShapeSpatial2D::_get_default_debug_color() const {
	return Color(1, 0, 0);
}

void CollisionShapeSpatial2D::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_PARENTED: {
			parent = ObjectUtils::cast_to<CollisionObjectSpatial2D>(get_parent());
			if (parent) {
				owner_id = parent->create_shape_owner(this);
				if (shape.is_valid()) {
					parent->shape_owner_add_shape(owner_id, shape);
				}
				_update_in_shape_owner();
			}
		} break;

		case NOTIFICATION_ENTER_TREE: {
			if (parent) {
				_update_in_shape_owner();
			}
		} break;

		case NOTIFICATION_LOCAL_TRANSFORM_CHANGED: {
			if (parent) {
				_update_in_shape_owner(true);
			}
		} break;

		case NOTIFICATION_UNPARENTED: {
			if (parent) {
				parent->remove_shape_owner(owner_id);
			}
			owner_id = 0;
			parent = nullptr;
		} break;
	}
}

Transform2D CollisionShapeSpatial2D::get_transform_2d() const {
	Transform3D tr = get_transform();

	return Transform2D(
		tr.basis[0][0], tr.basis[0][1],
		tr.basis[1][0], tr.basis[1][1],
		tr.origin[0], tr.origin[1]
	);
}

void CollisionShapeSpatial2D::set_shape(const Ref<Shape2D> &p_shape) {
	if (p_shape == shape) {
		return;
	}
	if (shape.is_valid()) {
		shape->disconnect("changed", Callable(this, "_shape_changed"));
	}
	shape = p_shape;
	update_gizmos();
	if (parent) {
		parent->shape_owner_clear_shapes(owner_id);
		if (shape.is_valid()) {
			parent->shape_owner_add_shape(owner_id, shape);
		}
		_update_in_shape_owner();
	}

	if (shape.is_valid()) {
		shape->connect("changed", Callable(this, "_shape_changed"));
	}

	update_configuration_warnings();
}

Ref<Shape2D> CollisionShapeSpatial2D::get_shape() const {
	return shape;
}

PackedStringArray CollisionShapeSpatial2D::_get_configuration_warnings() const {
	PackedStringArray warnings = Node::_get_configuration_warnings();

	if (!ObjectUtils::cast_to<CollisionObjectSpatial2D>(get_parent())) {
		warnings.push_back(tr("CollisionShapeSpatial2D only serves to provide a collision shape to a CollisionObjectSpatial2D derived node. Please only use it as a child of Area2D, StaticBody2D, RigidBody2D, CharacterBody2D, etc. to give them a shape."));
	}
	if (!shape.is_valid()) {
		warnings.push_back(tr("A shape must be provided for CollisionShapeSpatial2D to function. Please create a shape resource for it!"));
	}
	if (one_way_collision && ObjectUtils::cast_to<Area2D>(get_parent())) {
		warnings.push_back(tr("The One Way Collision property will be ignored when the parent is an Area2D."));
	}

	Ref<ConvexPolygonShape2D> convex = shape;
	Ref<ConcavePolygonShape2D> concave = shape;
	if (convex.is_valid() || concave.is_valid()) {
		warnings.push_back(tr("Polygon-based shapes are not meant be used nor edited directly through the CollisionShapeSpatial2D node. Please use the CollisionPolygon2D node instead."));
	}

	return warnings;
}

void CollisionShapeSpatial2D::set_disabled(bool p_disabled) {
	disabled = p_disabled;
	update_gizmos();
	if (parent) {
		parent->shape_owner_set_disabled(owner_id, p_disabled);
	}
}

bool CollisionShapeSpatial2D::is_disabled() const {
	return disabled;
}

void CollisionShapeSpatial2D::set_one_way_collision(bool p_enable) {
	one_way_collision = p_enable;
	update_gizmos();
	if (parent) {
		parent->shape_owner_set_one_way_collision(owner_id, p_enable);
	}
	update_configuration_warnings();
}

bool CollisionShapeSpatial2D::is_one_way_collision_enabled() const {
	return one_way_collision;
}

void CollisionShapeSpatial2D::set_one_way_collision_margin(real_t p_margin) {
	one_way_collision_margin = p_margin;
	if (parent) {
		parent->shape_owner_set_one_way_collision_margin(owner_id, one_way_collision_margin);
	}
}

real_t CollisionShapeSpatial2D::get_one_way_collision_margin() const {
	return one_way_collision_margin;
}

void CollisionShapeSpatial2D::set_debug_color(const Color &p_color) {
	debug_color = p_color;
	update_gizmos();
}

Color CollisionShapeSpatial2D::get_debug_color() const {
	return debug_color;
}

bool CollisionShapeSpatial2D::_property_can_revert(const StringName &p_name) const {
	if (p_name == StringName("debug_color")) {
		return true;
	}
	return false;
}

bool CollisionShapeSpatial2D::_property_get_revert(const StringName &p_name, Variant &r_property) const {
	if (p_name == StringName("debug_color")) {
		r_property = _get_default_debug_color();
		return true;
	}
	return false;
}

void CollisionShapeSpatial2D::_validate_property(PropertyInfo &p_property) const {
	if (p_property.name == StringName("debug_color")) {
		if (debug_color == _get_default_debug_color()) {
			p_property.usage = PROPERTY_USAGE_DEFAULT & ~PROPERTY_USAGE_STORAGE;
		} else {
			p_property.usage = PROPERTY_USAGE_DEFAULT;
		}
	}
}

void CollisionShapeSpatial2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_shape", "shape"), &CollisionShapeSpatial2D::set_shape);
	ClassDB::bind_method(D_METHOD("get_shape"), &CollisionShapeSpatial2D::get_shape);
	ClassDB::bind_method(D_METHOD("set_disabled", "disabled"), &CollisionShapeSpatial2D::set_disabled);
	ClassDB::bind_method(D_METHOD("is_disabled"), &CollisionShapeSpatial2D::is_disabled);
	ClassDB::bind_method(D_METHOD("set_one_way_collision", "enabled"), &CollisionShapeSpatial2D::set_one_way_collision);
	ClassDB::bind_method(D_METHOD("is_one_way_collision_enabled"), &CollisionShapeSpatial2D::is_one_way_collision_enabled);
	ClassDB::bind_method(D_METHOD("set_one_way_collision_margin", "margin"), &CollisionShapeSpatial2D::set_one_way_collision_margin);
	ClassDB::bind_method(D_METHOD("get_one_way_collision_margin"), &CollisionShapeSpatial2D::get_one_way_collision_margin);
	ClassDB::bind_method(D_METHOD("set_debug_color", "color"), &CollisionShapeSpatial2D::set_debug_color);
	ClassDB::bind_method(D_METHOD("get_debug_color"), &CollisionShapeSpatial2D::get_debug_color);
	ClassDB::bind_method(D_METHOD("_shape_changed"), &CollisionShapeSpatial2D::_shape_changed);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "shape", PROPERTY_HINT_RESOURCE_TYPE, "Shape2D"), "set_shape", "get_shape");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "disabled"), "set_disabled", "is_disabled");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "one_way_collision"), "set_one_way_collision", "is_one_way_collision_enabled");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "one_way_collision_margin", PROPERTY_HINT_RANGE, "0,128,0.1,suffix:px"), "set_one_way_collision_margin", "get_one_way_collision_margin");
	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "debug_color"), "set_debug_color", "get_debug_color");
}

CollisionShapeSpatial2D::CollisionShapeSpatial2D() {
	set_notify_local_transform(true);
	shape = Ref<Shape2D>();
	debug_color = _get_default_debug_color();
}