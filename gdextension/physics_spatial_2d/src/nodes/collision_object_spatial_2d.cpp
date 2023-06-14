#include "collision_object_spatial_2d.h"
#include "stdio.h"
#include <iostream>

#include <godot_cpp/classes/world2d.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void CollisionObjectSpatial2D::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			Transform2D gl_transform = get_global_transform_2d();

			if (area) {
				PhysicsServer2D::get_singleton()->area_set_transform(rid, gl_transform);
			} else {
				PhysicsServer2D::get_singleton()->body_set_state(rid, PhysicsServer2D::BODY_STATE_TRANSFORM, gl_transform);
			}

			bool disabled = !is_enabled();

			if (disabled && (disable_mode != DISABLE_MODE_REMOVE)) {
				_apply_disabled();
			}

			if (!disabled || (disable_mode != DISABLE_MODE_REMOVE)) {
				Ref<World2D> world_ref = get_world_2d();
				ERR_FAIL_COND(!world_ref.is_valid());
				RID space = world_ref->get_space();
				if (area) {
					PhysicsServer2D::get_singleton()->area_set_space(rid, space);
				} else {
					PhysicsServer2D::get_singleton()->body_set_space(rid, space);
				}
			}

			// _update_pickable();
		} break;

		// case NOTIFICATION_ENTER_CANVAS: {
		// 	if (area) {
		// 		PhysicsServer2D::get_singleton()->area_attach_canvas_instance_id(rid, get_canvas_layer_instance_id());
		// 	} else {
		// 		PhysicsServer2D::get_singleton()->body_attach_canvas_instance_id(rid, get_canvas_layer_instance_id());
		// 	}
		// } break;

		case NOTIFICATION_VISIBILITY_CHANGED: {
			// _update_pickable();
		} break;

		case NOTIFICATION_TRANSFORM_CHANGED: {
			if (only_update_transform_changes) {
				return;
			}

			Transform2D gl_transform = get_global_transform_2d();

			if (area) {
				PhysicsServer2D::get_singleton()->area_set_transform(rid, gl_transform);
			} else {
				PhysicsServer2D::get_singleton()->body_set_state(rid, PhysicsServer2D::BODY_STATE_TRANSFORM, gl_transform);
			}
		} break;

		case NOTIFICATION_EXIT_TREE: {
			bool disabled = !is_enabled();

			if (!disabled || (disable_mode != DISABLE_MODE_REMOVE)) {
				if (callback_lock > 0) {
					ERR_PRINT("Removing a CollisionObject node during a physics callback is not allowed and will cause undesired behavior. Remove with call_deferred() instead.");
				} else {
					if (area) {
						PhysicsServer2D::get_singleton()->area_set_space(rid, RID());
					} else {
						PhysicsServer2D::get_singleton()->body_set_space(rid, RID());
					}
				}
			}

			if (disabled && (disable_mode != DISABLE_MODE_REMOVE)) {
				_apply_enabled();
			}
		} break;

		// case NOTIFICATION_EXIT_CANVAS: {
		// 	if (area) {
		// 		PhysicsServer2D::get_singleton()->area_attach_canvas_instance_id(rid, ObjectID());
		// 	} else {
		// 		PhysicsServer2D::get_singleton()->body_attach_canvas_instance_id(rid, ObjectID());
		// 	}
		// } break;

		case NOTIFICATION_DISABLED: {
			_apply_disabled();
		} break;

		case NOTIFICATION_ENABLED: {
			_apply_enabled();
		} break;
	}
}

Transform2D CollisionObjectSpatial2D::get_transform_2d() const {
	Transform3D tr = get_transform();

	return Transform2D(
		tr.basis[0][0], -tr.basis[0][1],
		-tr.basis[1][0], tr.basis[1][1],
		tr.origin[0], tr.origin[1]
	);
}

void CollisionObjectSpatial2D::set_transform_2d(const Transform2D &p_transform) {
	Transform3D tr = get_transform();

	tr.basis[0][0] = p_transform[0][0];
	tr.basis[0][1] = -p_transform[0][1];
	tr.basis[1][0] = -p_transform[1][0];
	tr.basis[1][1] = p_transform[1][1];
	tr.origin[0] = p_transform[2][0];
	tr.origin[1] = p_transform[2][1];

	set_transform(tr);
}

Transform2D CollisionObjectSpatial2D::get_global_transform_2d() const {
	Transform3D gltr = get_global_transform();

	return Transform2D(
		gltr.basis[0][0], -gltr.basis[0][1],
		-gltr.basis[1][0], gltr.basis[1][1],
		gltr.origin[0], gltr.origin[1]
	);
}

void CollisionObjectSpatial2D::set_global_transform_2d(const Transform2D &p_transform) {
	Transform3D gltr = get_global_transform();

	gltr.basis[0][0] = p_transform[0][0];
	gltr.basis[0][1] = -p_transform[0][1];
	gltr.basis[1][0] = -p_transform[1][0];
	gltr.basis[1][1] = p_transform[1][1];
	gltr.origin[0] = p_transform[2][0];
	gltr.origin[1] = p_transform[2][1];

	set_global_transform(gltr);
}

void CollisionObjectSpatial2D::set_collision_layer(uint32_t p_layer) {
	collision_layer = p_layer;
	if (area) {
		PhysicsServer2D::get_singleton()->area_set_collision_layer(get_rid(), p_layer);
	} else {
		PhysicsServer2D::get_singleton()->body_set_collision_layer(get_rid(), p_layer);
	}
}

uint32_t CollisionObjectSpatial2D::get_collision_layer() const {
	return collision_layer;
}

void CollisionObjectSpatial2D::set_collision_mask(uint32_t p_mask) {
	collision_mask = p_mask;
	if (area) {
		PhysicsServer2D::get_singleton()->area_set_collision_mask(get_rid(), p_mask);
	} else {
		PhysicsServer2D::get_singleton()->body_set_collision_mask(get_rid(), p_mask);
	}
}

uint32_t CollisionObjectSpatial2D::get_collision_mask() const {
	return collision_mask;
}

void CollisionObjectSpatial2D::set_collision_layer_value(int p_layer_number, bool p_value) {
	ERR_FAIL_COND_MSG(p_layer_number < 1, "Collision layer number must be between 1 and 32 inclusive.");
	ERR_FAIL_COND_MSG(p_layer_number > 32, "Collision layer number must be between 1 and 32 inclusive.");
	uint32_t collision_layer_new = get_collision_layer();
	if (p_value) {
		collision_layer_new |= 1 << (p_layer_number - 1);
	} else {
		collision_layer_new &= ~(1 << (p_layer_number - 1));
	}
	set_collision_layer(collision_layer_new);
}

bool CollisionObjectSpatial2D::get_collision_layer_value(int p_layer_number) const {
	ERR_FAIL_COND_V_MSG(p_layer_number < 1, false, "Collision layer number must be between 1 and 32 inclusive.");
	ERR_FAIL_COND_V_MSG(p_layer_number > 32, false, "Collision layer number must be between 1 and 32 inclusive.");
	return get_collision_layer() & (1 << (p_layer_number - 1));
}

void CollisionObjectSpatial2D::set_collision_mask_value(int p_layer_number, bool p_value) {
	ERR_FAIL_COND_MSG(p_layer_number < 1, "Collision layer number must be between 1 and 32 inclusive.");
	ERR_FAIL_COND_MSG(p_layer_number > 32, "Collision layer number must be between 1 and 32 inclusive.");
	uint32_t mask = get_collision_mask();
	if (p_value) {
		mask |= 1 << (p_layer_number - 1);
	} else {
		mask &= ~(1 << (p_layer_number - 1));
	}
	set_collision_mask(mask);
}

bool CollisionObjectSpatial2D::get_collision_mask_value(int p_layer_number) const {
	ERR_FAIL_COND_V_MSG(p_layer_number < 1, false, "Collision layer number must be between 1 and 32 inclusive.");
	ERR_FAIL_COND_V_MSG(p_layer_number > 32, false, "Collision layer number must be between 1 and 32 inclusive.");
	return get_collision_mask() & (1 << (p_layer_number - 1));
}

void CollisionObjectSpatial2D::set_collision_priority(real_t p_priority) {
	collision_priority = p_priority;
	if (!area) {
		PhysicsServer2D::get_singleton()->body_set_collision_priority(get_rid(), p_priority);
	}
}

real_t CollisionObjectSpatial2D::get_collision_priority() const {
	return collision_priority;
}

void CollisionObjectSpatial2D::set_disable_mode(DisableMode p_mode) {
	if (disable_mode == p_mode) {
		return;
	}

	bool disabled = is_inside_tree() && !is_enabled();

	if (disabled) {
		// Cancel previous disable mode.
		_apply_enabled();
	}

	disable_mode = p_mode;

	if (disabled) {
		// Apply new disable mode.
		_apply_disabled();
	}
}

CollisionObjectSpatial2D::DisableMode CollisionObjectSpatial2D::get_disable_mode() const {
	return disable_mode;
}

void CollisionObjectSpatial2D::_apply_disabled() {
	switch (disable_mode) {
		case DISABLE_MODE_REMOVE: {
			if (is_inside_tree()) {
				if (callback_lock > 0) {
					ERR_PRINT("Disabling a CollisionObject node during a physics callback is not allowed and will cause undesired behavior. Disable with call_deferred() instead.");
				} else {
					if (area) {
						PhysicsServer2D::get_singleton()->area_set_space(rid, RID());
					} else {
						PhysicsServer2D::get_singleton()->body_set_space(rid, RID());
					}
				}
			}
		} break;

		case DISABLE_MODE_MAKE_STATIC: {
			if (!area && (body_mode != PhysicsServer2D::BODY_MODE_STATIC)) {
				PhysicsServer2D::get_singleton()->body_set_mode(rid, PhysicsServer2D::BODY_MODE_STATIC);
			}
		} break;

		case DISABLE_MODE_KEEP_ACTIVE: {
			// Nothing to do.
		} break;
	}
}

void CollisionObjectSpatial2D::_apply_enabled() {
	switch (disable_mode) {
		case DISABLE_MODE_REMOVE: {
			if (is_inside_tree()) {
				RID space = get_world_2d()->get_space();
				if (area) {
					PhysicsServer2D::get_singleton()->area_set_space(rid, space);
				} else {
					PhysicsServer2D::get_singleton()->body_set_space(rid, space);
				}
			}
		} break;

		case DISABLE_MODE_MAKE_STATIC: {
			if (!area && (body_mode != PhysicsServer2D::BODY_MODE_STATIC)) {
				PhysicsServer2D::get_singleton()->body_set_mode(rid, body_mode);
			}
		} break;

		case DISABLE_MODE_KEEP_ACTIVE: {
			// Nothing to do.
		} break;
	}
}

uint32_t CollisionObjectSpatial2D::create_shape_owner(Object *p_owner) {
	ShapeData sd;
	uint32_t id;

	if (shapes.size() == 0) {
		id = 0;
	} else {
		id = shapes.back()->key() + 1;
	}

	sd.owner_id = p_owner ? p_owner->get_instance_id() : ObjectID();

	shapes[id] = sd;

	return id;
}

void CollisionObjectSpatial2D::remove_shape_owner(uint32_t owner) {
	ERR_FAIL_COND(!shapes.has(owner));

	shape_owner_clear_shapes(owner);

	shapes.erase(owner);
}

void CollisionObjectSpatial2D::shape_owner_set_disabled(uint32_t p_owner, bool p_disabled) {
	ERR_FAIL_COND(!shapes.has(p_owner));

	ShapeData &sd = shapes[p_owner];
	sd.disabled = p_disabled;
	for (int i = 0; i < sd.shapes.size(); i++) {
		if (area) {
			PhysicsServer2D::get_singleton()->area_set_shape_disabled(rid, sd.shapes[i].index, p_disabled);
		} else {
			PhysicsServer2D::get_singleton()->body_set_shape_disabled(rid, sd.shapes[i].index, p_disabled);
		}
	}
}

bool CollisionObjectSpatial2D::is_shape_owner_disabled(uint32_t p_owner) const {
	ERR_FAIL_COND_V(!shapes.has(p_owner), false);

	return shapes[p_owner].disabled;
}

void CollisionObjectSpatial2D::shape_owner_set_one_way_collision(uint32_t p_owner, bool p_enable) {
	if (area) {
		return; //not for areas
	}

	ERR_FAIL_COND(!shapes.has(p_owner));

	ShapeData &sd = shapes[p_owner];
	sd.one_way_collision = p_enable;
	for (int i = 0; i < sd.shapes.size(); i++) {
		PhysicsServer2D::get_singleton()->body_set_shape_as_one_way_collision(rid, sd.shapes[i].index, sd.one_way_collision, sd.one_way_collision_margin);
	}
}

bool CollisionObjectSpatial2D::is_shape_owner_one_way_collision_enabled(uint32_t p_owner) const {
	ERR_FAIL_COND_V(!shapes.has(p_owner), false);

	return shapes[p_owner].one_way_collision;
}

void CollisionObjectSpatial2D::shape_owner_set_one_way_collision_margin(uint32_t p_owner, real_t p_margin) {
	if (area) {
		return; //not for areas
	}

	ERR_FAIL_COND(!shapes.has(p_owner));

	ShapeData &sd = shapes[p_owner];
	sd.one_way_collision_margin = p_margin;
	for (int i = 0; i < sd.shapes.size(); i++) {
		PhysicsServer2D::get_singleton()->body_set_shape_as_one_way_collision(rid, sd.shapes[i].index, sd.one_way_collision, sd.one_way_collision_margin);
	}
}

real_t CollisionObjectSpatial2D::get_shape_owner_one_way_collision_margin(uint32_t p_owner) const {
	ERR_FAIL_COND_V(!shapes.has(p_owner), 0);

	return shapes[p_owner].one_way_collision_margin;
}

void CollisionObjectSpatial2D::get_shape_owners(List<uint32_t> *r_owners) {
	for (const KeyValue<uint32_t, ShapeData> &E : shapes) {
		r_owners->push_back(E.key);
	}
}

PackedInt32Array CollisionObjectSpatial2D::_get_shape_owners() {
	PackedInt32Array ret;
	for (const KeyValue<uint32_t, ShapeData> &E : shapes) {
		ret.push_back(E.key);
	}

	return ret;
}

void CollisionObjectSpatial2D::shape_owner_set_transform(uint32_t p_owner, const Transform2D &p_transform) {
	ERR_FAIL_COND(!shapes.has(p_owner));

	ShapeData &sd = shapes[p_owner];

	sd.xform = p_transform;
	for (int i = 0; i < sd.shapes.size(); i++) {
		if (area) {
			PhysicsServer2D::get_singleton()->area_set_shape_transform(rid, sd.shapes[i].index, sd.xform);
		} else {
			PhysicsServer2D::get_singleton()->body_set_shape_transform(rid, sd.shapes[i].index, sd.xform);
		}
	}
}

Transform2D CollisionObjectSpatial2D::shape_owner_get_transform(uint32_t p_owner) const {
	ERR_FAIL_COND_V(!shapes.has(p_owner), Transform2D());

	return shapes[p_owner].xform;
}

Object *CollisionObjectSpatial2D::shape_owner_get_owner(uint32_t p_owner) const {
	ERR_FAIL_COND_V(!shapes.has(p_owner), nullptr);

	return ObjectDB::get_instance(shapes[p_owner].owner_id);
}

void CollisionObjectSpatial2D::shape_owner_add_shape(uint32_t p_owner, const Ref<Shape2D> &p_shape) {
	ERR_FAIL_COND(!shapes.has(p_owner));
	ERR_FAIL_COND(p_shape.is_null());

	ShapeData &sd = shapes[p_owner];
	ShapeData::Shape s;
	s.index = total_subshapes;
	s.shape = p_shape;
	if (area) {
		PhysicsServer2D::get_singleton()->area_add_shape(rid, p_shape->get_rid(), sd.xform, sd.disabled);
	} else {
		PhysicsServer2D::get_singleton()->body_add_shape(rid, p_shape->get_rid(), sd.xform, sd.disabled);
	}
	sd.shapes.push_back(s);

	total_subshapes++;
}

int CollisionObjectSpatial2D::shape_owner_get_shape_count(uint32_t p_owner) const {
	ERR_FAIL_COND_V(!shapes.has(p_owner), 0);

	return shapes[p_owner].shapes.size();
}

Ref<Shape2D> CollisionObjectSpatial2D::shape_owner_get_shape(uint32_t p_owner, int p_shape) const {
	ERR_FAIL_COND_V(!shapes.has(p_owner), Ref<Shape2D>());
	ERR_FAIL_INDEX_V(p_shape, shapes[p_owner].shapes.size(), Ref<Shape2D>());

	return shapes[p_owner].shapes[p_shape].shape;
}

int CollisionObjectSpatial2D::shape_owner_get_shape_index(uint32_t p_owner, int p_shape) const {
	ERR_FAIL_COND_V(!shapes.has(p_owner), -1);
	ERR_FAIL_INDEX_V(p_shape, shapes[p_owner].shapes.size(), -1);

	return shapes[p_owner].shapes[p_shape].index;
}

void CollisionObjectSpatial2D::shape_owner_remove_shape(uint32_t p_owner, int p_shape) {
	ERR_FAIL_COND(!shapes.has(p_owner));
	ERR_FAIL_INDEX(p_shape, shapes[p_owner].shapes.size());

	int index_to_remove = shapes[p_owner].shapes[p_shape].index;
	if (area) {
		PhysicsServer2D::get_singleton()->area_remove_shape(rid, index_to_remove);
	} else {
		PhysicsServer2D::get_singleton()->body_remove_shape(rid, index_to_remove);
	}

	shapes[p_owner].shapes.remove_at(p_shape);

	for (KeyValue<uint32_t, ShapeData> &E : shapes) {
		for (int i = 0; i < E.value.shapes.size(); i++) {
			if (E.value.shapes[i].index > index_to_remove) {
				E.value.shapes.write[i].index -= 1;
			}
		}
	}

	total_subshapes--;
}

void CollisionObjectSpatial2D::shape_owner_clear_shapes(uint32_t p_owner) {
	ERR_FAIL_COND(!shapes.has(p_owner));

	while (shape_owner_get_shape_count(p_owner) > 0) {
		shape_owner_remove_shape(p_owner, 0);
	}
}

uint32_t CollisionObjectSpatial2D::shape_find_owner(int p_shape_index) const {
	ERR_FAIL_INDEX_V(p_shape_index, total_subshapes, UINT32_MAX);

	for (const KeyValue<uint32_t, ShapeData> &E : shapes) {
		for (int i = 0; i < E.value.shapes.size(); i++) {
			if (E.value.shapes[i].index == p_shape_index) {
				return E.key;
			}
		}
	}

	//in theory it should be unreachable
	ERR_FAIL_V_MSG(UINT32_MAX, "Can't find owner for shape index " + itos(p_shape_index) + ".");
}

void CollisionObjectSpatial2D::set_pickable(bool p_enabled) {
	if (pickable == p_enabled) {
		return;
	}

	pickable = p_enabled;
	// _update_pickable();
}

bool CollisionObjectSpatial2D::is_pickable() const {
	return pickable;
}

// void CollisionObjectSpatial2D::_input_event_call(Viewport *p_viewport, const Ref<InputEvent> &p_input_event, int p_shape) {
// 	GDVIRTUAL_CALL(_input_event, p_viewport, p_input_event, p_shape);
// 	emit_signal(SceneStringNames::get_singleton()->input_event, p_viewport, p_input_event, p_shape);
// }

// void CollisionObjectSpatial2D::_mouse_enter() {
// 	if (get_script_instance()) {
// 		get_script_instance()->call(SceneStringNames::get_singleton()->_mouse_enter);
// 	}
// 	emit_signal(SceneStringNames::get_singleton()->mouse_entered);
// }

// void CollisionObjectSpatial2D::_mouse_exit() {
// 	if (get_script_instance()) {
// 		get_script_instance()->call(SceneStringNames::get_singleton()->_mouse_exit);
// 	}
// 	emit_signal(SceneStringNames::get_singleton()->mouse_exited);
// }

// void CollisionObjectSpatial2D::_mouse_shape_enter(int p_shape) {
// 	if (get_script_instance()) {
// 		get_script_instance()->call(SceneStringNames::get_singleton()->_mouse_shape_enter, p_shape);
// 	}
// 	emit_signal(SceneStringNames::get_singleton()->mouse_shape_entered, p_shape);
// }

// void CollisionObjectSpatial2D::_mouse_shape_exit(int p_shape) {
// 	if (get_script_instance()) {
// 		get_script_instance()->call(SceneStringNames::get_singleton()->_mouse_shape_exit, p_shape);
// 	}
// 	emit_signal(SceneStringNames::get_singleton()->mouse_shape_exited, p_shape);
// }

void CollisionObjectSpatial2D::set_only_update_transform_changes(bool p_enable) {
	only_update_transform_changes = p_enable;
}

bool CollisionObjectSpatial2D::is_only_update_transform_changes_enabled() const {
	return only_update_transform_changes;
}

void CollisionObjectSpatial2D::set_body_mode(PhysicsServer2D::BodyMode p_mode) {
	ERR_FAIL_COND(area);

	if (body_mode == p_mode) {
		return;
	}

	body_mode = p_mode;

	if (is_inside_tree() && !is_enabled() && (disable_mode == DISABLE_MODE_MAKE_STATIC)) {
		return;
	}

	PhysicsServer2D::get_singleton()->body_set_mode(rid, p_mode);
}

// void CollisionObjectSpatial2D::_update_pickable() {
// 	if (!is_inside_tree()) {
// 		return;
// 	}

// 	bool is_pickable = pickable && is_visible_in_tree();
// 	if (area) {
// 		PhysicsServer2D::get_singleton()->area_set_pickable(rid, is_pickable);
// 	} else {
// 		PhysicsServer2D::get_singleton()->body_set_pickable(rid, is_pickable);
// 	}
// }

PackedStringArray CollisionObjectSpatial2D::_get_configuration_warnings() const {
	PackedStringArray warnings = Node::_get_configuration_warnings();

	if (shapes.is_empty()) {
		warnings.push_back(tr("This node has no shape, so it can't collide or interact with other objects.\nConsider adding a CollisionShape2D or CollisionPolygon2D as a child to define its shape."));
	}

	return warnings;
}

void CollisionObjectSpatial2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_rid"), &CollisionObjectSpatial2D::get_rid);
	ClassDB::bind_method(D_METHOD("set_collision_layer", "layer"), &CollisionObjectSpatial2D::set_collision_layer);
	ClassDB::bind_method(D_METHOD("get_collision_layer"), &CollisionObjectSpatial2D::get_collision_layer);
	ClassDB::bind_method(D_METHOD("set_collision_mask", "mask"), &CollisionObjectSpatial2D::set_collision_mask);
	ClassDB::bind_method(D_METHOD("get_collision_mask"), &CollisionObjectSpatial2D::get_collision_mask);
	ClassDB::bind_method(D_METHOD("set_collision_layer_value", "layer_number", "value"), &CollisionObjectSpatial2D::set_collision_layer_value);
	ClassDB::bind_method(D_METHOD("get_collision_layer_value", "layer_number"), &CollisionObjectSpatial2D::get_collision_layer_value);
	ClassDB::bind_method(D_METHOD("set_collision_mask_value", "layer_number", "value"), &CollisionObjectSpatial2D::set_collision_mask_value);
	ClassDB::bind_method(D_METHOD("get_collision_mask_value", "layer_number"), &CollisionObjectSpatial2D::get_collision_mask_value);
	ClassDB::bind_method(D_METHOD("set_collision_priority", "priority"), &CollisionObjectSpatial2D::set_collision_priority);
	ClassDB::bind_method(D_METHOD("get_collision_priority"), &CollisionObjectSpatial2D::get_collision_priority);
	ClassDB::bind_method(D_METHOD("set_disable_mode", "mode"), &CollisionObjectSpatial2D::set_disable_mode);
	ClassDB::bind_method(D_METHOD("get_disable_mode"), &CollisionObjectSpatial2D::get_disable_mode);
	ClassDB::bind_method(D_METHOD("set_pickable", "enabled"), &CollisionObjectSpatial2D::set_pickable);
	ClassDB::bind_method(D_METHOD("is_pickable"), &CollisionObjectSpatial2D::is_pickable);
	ClassDB::bind_method(D_METHOD("create_shape_owner", "owner"), &CollisionObjectSpatial2D::create_shape_owner);
	ClassDB::bind_method(D_METHOD("remove_shape_owner", "owner_id"), &CollisionObjectSpatial2D::remove_shape_owner);
	ClassDB::bind_method(D_METHOD("get_shape_owners"), &CollisionObjectSpatial2D::_get_shape_owners);
	ClassDB::bind_method(D_METHOD("shape_owner_set_transform", "owner_id", "transform"), &CollisionObjectSpatial2D::shape_owner_set_transform);
	ClassDB::bind_method(D_METHOD("shape_owner_get_transform", "owner_id"), &CollisionObjectSpatial2D::shape_owner_get_transform);
	ClassDB::bind_method(D_METHOD("shape_owner_get_owner", "owner_id"), &CollisionObjectSpatial2D::shape_owner_get_owner);
	ClassDB::bind_method(D_METHOD("shape_owner_set_disabled", "owner_id", "disabled"), &CollisionObjectSpatial2D::shape_owner_set_disabled);
	ClassDB::bind_method(D_METHOD("is_shape_owner_disabled", "owner_id"), &CollisionObjectSpatial2D::is_shape_owner_disabled);
	ClassDB::bind_method(D_METHOD("shape_owner_set_one_way_collision", "owner_id", "enable"), &CollisionObjectSpatial2D::shape_owner_set_one_way_collision);
	ClassDB::bind_method(D_METHOD("is_shape_owner_one_way_collision_enabled", "owner_id"), &CollisionObjectSpatial2D::is_shape_owner_one_way_collision_enabled);
	ClassDB::bind_method(D_METHOD("shape_owner_set_one_way_collision_margin", "owner_id", "margin"), &CollisionObjectSpatial2D::shape_owner_set_one_way_collision_margin);
	ClassDB::bind_method(D_METHOD("get_shape_owner_one_way_collision_margin", "owner_id"), &CollisionObjectSpatial2D::get_shape_owner_one_way_collision_margin);
	ClassDB::bind_method(D_METHOD("shape_owner_add_shape", "owner_id", "shape"), &CollisionObjectSpatial2D::shape_owner_add_shape);
	ClassDB::bind_method(D_METHOD("shape_owner_get_shape_count", "owner_id"), &CollisionObjectSpatial2D::shape_owner_get_shape_count);
	ClassDB::bind_method(D_METHOD("shape_owner_get_shape", "owner_id", "shape_id"), &CollisionObjectSpatial2D::shape_owner_get_shape);
	ClassDB::bind_method(D_METHOD("shape_owner_get_shape_index", "owner_id", "shape_id"), &CollisionObjectSpatial2D::shape_owner_get_shape_index);
	ClassDB::bind_method(D_METHOD("shape_owner_remove_shape", "owner_id", "shape_id"), &CollisionObjectSpatial2D::shape_owner_remove_shape);
	ClassDB::bind_method(D_METHOD("shape_owner_clear_shapes", "owner_id"), &CollisionObjectSpatial2D::shape_owner_clear_shapes);
	ClassDB::bind_method(D_METHOD("shape_find_owner", "shape_index"), &CollisionObjectSpatial2D::shape_find_owner);

	// GDVIRTUAL_BIND(_input_event, "viewport", "event", "shape_idx");
	// GDVIRTUAL_BIND(_mouse_enter);
	// GDVIRTUAL_BIND(_mouse_exit);
	// GDVIRTUAL_BIND(_mouse_shape_enter, "shape_idx");
	// GDVIRTUAL_BIND(_mouse_shape_exit, "shape_idx");

	// ADD_SIGNAL(MethodInfo("input_event", PropertyInfo(Variant::OBJECT, "viewport", PROPERTY_HINT_RESOURCE_TYPE, "Node"), PropertyInfo(Variant::OBJECT, "event", PROPERTY_HINT_RESOURCE_TYPE, "InputEvent"), PropertyInfo(Variant::INT, "shape_idx")));
	// ADD_SIGNAL(MethodInfo("mouse_entered"));
	// ADD_SIGNAL(MethodInfo("mouse_exited"));
	// ADD_SIGNAL(MethodInfo("mouse_shape_entered", PropertyInfo(Variant::INT, "shape_idx")));
	// ADD_SIGNAL(MethodInfo("mouse_shape_exited", PropertyInfo(Variant::INT, "shape_idx")));

	ADD_PROPERTY(PropertyInfo(Variant::INT, "disable_mode", PROPERTY_HINT_ENUM, "Remove,Make Static,Keep Active"), "set_disable_mode", "get_disable_mode");

	ADD_GROUP("Collision", "collision_");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "collision_layer", PROPERTY_HINT_LAYERS_2D_PHYSICS), "set_collision_layer", "get_collision_layer");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "collision_mask", PROPERTY_HINT_LAYERS_2D_PHYSICS), "set_collision_mask", "get_collision_mask");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "collision_priority"), "set_collision_priority", "get_collision_priority");

	// ADD_GROUP("Input", "input_");
	// ADD_PROPERTY(PropertyInfo(Variant::BOOL, "input_pickable"), "set_pickable", "is_pickable");

	BIND_ENUM_CONSTANT(DISABLE_MODE_REMOVE);
	BIND_ENUM_CONSTANT(DISABLE_MODE_MAKE_STATIC);
	BIND_ENUM_CONSTANT(DISABLE_MODE_KEEP_ACTIVE);
}

CollisionObjectSpatial2D::CollisionObjectSpatial2D(RID p_rid, bool p_area) {
	rid = p_rid;
	area = p_area;
	pickable = true;
	set_notify_transform(true);
	// set_hide_clip_children(true);
	total_subshapes = 0;
	only_update_transform_changes = false;

	if (p_area) {
		PhysicsServer2D::get_singleton()->area_attach_object_instance_id(rid, get_instance_id());
	} else {
		PhysicsServer2D::get_singleton()->body_attach_object_instance_id(rid, get_instance_id());
		PhysicsServer2D::get_singleton()->body_set_mode(rid, body_mode);
	}
}

CollisionObjectSpatial2D::CollisionObjectSpatial2D() {
	//owner=

	set_notify_transform(true);
}

CollisionObjectSpatial2D::~CollisionObjectSpatial2D() {
	ERR_FAIL_NULL(PhysicsServer2D::get_singleton());
	PhysicsServer2D::get_singleton()->free_rid(rid);
}