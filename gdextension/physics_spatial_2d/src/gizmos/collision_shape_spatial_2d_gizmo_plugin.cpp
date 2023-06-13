#include "collision_shape_spatial_2d_gizmo_plugin.h"
#include "../util/object.hpp"

#include <godot_cpp/classes/editor_settings.hpp>
#include <godot_cpp/classes/editor_interface.hpp>
#include <godot_cpp/classes/editor_undo_redo_manager.hpp>
#include <godot_cpp/classes/circle_shape2d.hpp>
#include <godot_cpp/classes/rectangle_shape2d.hpp>
#include <godot_cpp/classes/capsule_shape2d.hpp>
#include <godot_cpp/classes/separation_ray_shape2d.hpp>
#include <godot_cpp/classes/geometry3d.hpp>
#include <godot_cpp/classes/standard_material3d.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#define GP_STR(x) #x
#define GP_TOSTR(x) GP_STR(x)
#define DBG() UtilityFunctions::print(GP_TOSTR(__LINE__));

using namespace godot;

CollisionShapeSpatial2DGizmoPlugin::CollisionShapeSpatial2DGizmoPlugin() {
	plugin = nullptr;
}

void CollisionShapeSpatial2DGizmoPlugin::initialize() {
	ERR_FAIL_COND(!plugin);

	EditorInterface *interface = plugin->get_editor_interface();
	Ref<EditorSettings> settings = interface->get_editor_settings();

	const Color gizmo_color = settings->get_setting("editors/3d_gizmos/gizmo_colors/shape");
	create_material("shape_material", gizmo_color);
	const float gizmo_value = gizmo_color.get_v();
	const Color gizmo_color_disabled = Color(gizmo_value, gizmo_value, gizmo_value, 0.65);
	create_material("shape_material_disabled", gizmo_color_disabled);
	create_handle_material("handles");
}

bool CollisionShapeSpatial2DGizmoPlugin::_has_gizmo(Node3D *p_spatial) const {
	return ObjectUtils::cast_to<CollisionShapeSpatial2D>(p_spatial) != nullptr;
}

String CollisionShapeSpatial2DGizmoPlugin::_get_gizmo_name() const {
	return "CollisionShapeSpatial2D";
}

int CollisionShapeSpatial2DGizmoPlugin::_get_priority() const {
	return -1;
}

String CollisionShapeSpatial2DGizmoPlugin::_get_handle_name(const Ref<EditorNode3DGizmo> &p_gizmo, int p_id, bool p_secondary) const {
	const CollisionShapeSpatial2D *cs = ObjectUtils::cast_to<CollisionShapeSpatial2D>(p_gizmo->get_node_3d());

	Ref<Shape2D> s = cs->get_shape();
	if (s.is_null()) {
		return "";
	}

	if (Object::cast_to<CircleShape2D>(*s)) {
		return "Radius";
	}

	if (Object::cast_to<RectangleShape2D>(*s)) {
		return "Size";
	}

	if (Object::cast_to<CapsuleShape2D>(*s)) {
		return p_id == 0 ? "Radius" : "Height";
	}

	if (Object::cast_to<SeparationRayShape2D>(*s)) {
		return "Length";
	}

	return "";
}

Variant CollisionShapeSpatial2DGizmoPlugin::_get_handle_value(const Ref<EditorNode3DGizmo> &p_gizmo, int p_id, bool p_secondary) const {
	CollisionShapeSpatial2D *cs = ObjectUtils::cast_to<CollisionShapeSpatial2D>(p_gizmo->get_node_3d());

	Ref<Shape2D> s = cs->get_shape();
	if (s.is_null()) {
		return Variant();
	}

	if (Object::cast_to<CircleShape2D>(*s)) {
		Ref<CircleShape2D> ss = s;
		return ss->get_radius();
	}

	if (Object::cast_to<RectangleShape2D>(*s)) {
		Ref<RectangleShape2D> bs = s;
		return bs->get_size();
	}

	if (Object::cast_to<CapsuleShape2D>(*s)) {
		Ref<CapsuleShape2D> cs2 = s;
		return Vector2(cs2->get_radius(), cs2->get_height());
	}

	if (Object::cast_to<SeparationRayShape2D>(*s)) {
		Ref<SeparationRayShape2D> cs2 = s;
		return cs2->get_length();
	}

	return Variant();
}

void CollisionShapeSpatial2DGizmoPlugin::_set_handle(const Ref<EditorNode3DGizmo> &p_gizmo, int32_t p_id, bool p_secondary, Camera3D *p_camera, const Vector2 &p_point) {	
	CollisionShapeSpatial2D *cs = ObjectUtils::cast_to<CollisionShapeSpatial2D>(p_gizmo->get_node_3d());

	Ref<Shape2D> s = cs->get_shape();
	if (s.is_null()) {
		return;
	}

	Transform3D gt = cs->get_global_transform();
	Transform3D gi = gt.affine_inverse();

	Vector3 ray_from = p_camera->project_ray_origin(p_point);
	Vector3 ray_dir = p_camera->project_ray_normal(p_point);

	Vector3 sg[2] = { gi.xform(ray_from), gi.xform(ray_from + ray_dir * 4096) };

	if (Object::cast_to<CircleShape2D>(*s)) {
		Ref<CircleShape2D> ss = s;
		PackedVector3Array r = Geometry3D::get_singleton()->get_closest_points_between_segments(Vector3(), Vector3(4096, 0, 0), sg[0], sg[1]);
		Vector3 ra = r[0];
		Vector3 rb = r[1];
		float d = ra.x;
		// if (Node3DEditor::get_singleton()->is_snap_enabled()) {
		// 	d = Math::snapped(d, Node3DEditor::get_singleton()->get_translate_snap());
		// }

		if (d < 0.001) {
			d = 0.001;
		}

		ss->set_radius(d);
	}

	if (Object::cast_to<SeparationRayShape2D>(*s)) {
		Ref<SeparationRayShape2D> rs = s;
		PackedVector3Array r =Geometry3D::get_singleton()->get_closest_points_between_segments(Vector3(), Vector3(0, -4096, 0), sg[0], sg[1]);
		Vector3 ra = r[0];
		Vector3 rb = r[1];
		float d = -ra.y;
		// if (Node3DEditor::get_singleton()->is_snap_enabled()) {
		// 	d = Math::snapped(d, Node3DEditor::get_singleton()->get_translate_snap());
		// }

		if (d < 0.001) {
			d = 0.001;
		}

		rs->set_length(d);
	}

	if (Object::cast_to<RectangleShape2D>(*s)) {
		Vector3 axis;
		axis[p_id] = 1.0;
		Ref<RectangleShape2D> bs = s;
		PackedVector3Array r = Geometry3D::get_singleton()->get_closest_points_between_segments(Vector3(), axis * 4096, sg[0], sg[1]);
		Vector3 ra = r[0];
		Vector3 rb = r[1];
		float d = ra[p_id] * 2;
		// if (Node3DEditor::get_singleton()->is_snap_enabled()) {
		// 	d = Math::snapped(d, Node3DEditor::get_singleton()->get_translate_snap());
		// }

		if (d < 0.001) {
			d = 0.001;
		}

		Vector2 he = bs->get_size();
		he[p_id] = d;
		bs->set_size(he);
	}

	if (Object::cast_to<CapsuleShape2D>(*s)) {
		Vector3 axis;
		axis[p_id] = 1.0;
		Ref<CapsuleShape2D> cs2 = s;
		PackedVector3Array r = Geometry3D::get_singleton()->get_closest_points_between_segments(Vector3(), axis * 4096, sg[0], sg[1]);
		Vector3 ra = r[0];
		Vector3 rb = r[1];
		float d = axis.dot(ra);

		// if (Node3DEditor::get_singleton()->is_snap_enabled()) {
		// 	d = Math::snapped(d, Node3DEditor::get_singleton()->get_translate_snap());
		// }

		if (d < 0.001) {
			d = 0.001;
		}

		if (p_id == 0) {
			cs2->set_radius(d);
		} else if (p_id == 1) {
			cs2->set_height(d * 2.0);
		}
	}
}

void CollisionShapeSpatial2DGizmoPlugin::_commit_handle(const Ref<EditorNode3DGizmo> &p_gizmo, int p_id, bool p_secondary, const Variant &p_restore, bool p_cancel) {
	CollisionShapeSpatial2D *cs = ObjectUtils::cast_to<CollisionShapeSpatial2D>(p_gizmo->get_node_3d());

	Ref<Shape2D> s = cs->get_shape();
	if (s.is_null()) {
		return;
	}

	if (Object::cast_to<CircleShape2D>(*s)) {
		Ref<CircleShape2D> ss = s;
		if (p_cancel) {
			ss->set_radius(p_restore);
			return;
		}

		EditorUndoRedoManager *ur = plugin->get_undo_redo();
		ur->create_action(tr("Change Sphere Shape Radius"));
		ur->add_do_method(ss.ptr(), "set_radius", ss->get_radius());
		ur->add_undo_method(ss.ptr(), "set_radius", p_restore);
		ur->commit_action();
	}

	if (Object::cast_to<RectangleShape2D>(*s)) {
		Ref<RectangleShape2D> ss = s;
		if (p_cancel) {
			ss->set_size(p_restore);
			return;
		}

		EditorUndoRedoManager *ur = plugin->get_undo_redo();
		ur->create_action(tr("Change Box Shape Size"));
		ur->add_do_method(ss.ptr(), "set_size", ss->get_size());
		ur->add_undo_method(ss.ptr(), "set_size", p_restore);
		ur->commit_action();
	}

	if (Object::cast_to<CapsuleShape2D>(*s)) {
		Ref<CapsuleShape2D> ss = s;
		Vector2 values = p_restore;

		if (p_cancel) {
			ss->set_radius(values[0]);
			ss->set_height(values[1]);
			return;
		}

		EditorUndoRedoManager *ur = plugin->get_undo_redo();
		if (p_id == 0) {
			ur->create_action(tr("Change Capsule Shape Radius"));
			ur->add_do_method(ss.ptr(), "set_radius", ss->get_radius());
		} else {
			ur->create_action(tr("Change Capsule Shape Height"));
			ur->add_do_method(ss.ptr(), "set_height", ss->get_height());
		}
		ur->add_undo_method(ss.ptr(), "set_radius", values[0]);
		ur->add_undo_method(ss.ptr(), "set_height", values[1]);

		ur->commit_action();
	}

	if (Object::cast_to<SeparationRayShape2D>(*s)) {
		Ref<SeparationRayShape2D> ss = s;
		if (p_cancel) {
			ss->set_length(p_restore);
			return;
		}

		EditorUndoRedoManager *ur = plugin->get_undo_redo();
		ur->create_action(tr("Change Separation Ray Shape Length"));
		ur->add_do_method(ss.ptr(), "set_length", ss->get_length());
		ur->add_undo_method(ss.ptr(), "set_length", p_restore);
		ur->commit_action();
	}
}

void CollisionShapeSpatial2DGizmoPlugin::_redraw(const Ref<EditorNode3DGizmo> &p_gizmo) {
	EditorNode3DGizmo *gizmo = (EditorNode3DGizmo *)(p_gizmo.ptr());

	CollisionShapeSpatial2D *cs = ObjectUtils::cast_to<CollisionShapeSpatial2D>(gizmo->get_node_3d());

	gizmo->clear();

	Ref<Shape2D> s = cs->get_shape();
	if (s.is_null()) {
		return;
	}

	const Ref<Material> material =
			get_material(!cs->is_disabled() ? "shape_material" : "shape_material_disabled", gizmo);

	Ref<Material> handles_material = get_material("handles");

	if (Object::cast_to<CircleShape2D>(*s)) {
		Ref<CircleShape2D> sp = s;
		float r = sp->get_radius();

		PackedVector3Array points;

		for (int i = 0; i <= 360; i++) {
			float ra = Math::deg_to_rad((float)i);
			float rb = Math::deg_to_rad((float)i + 1);
			Vector2 a = Vector2(Math::sin(ra), Math::cos(ra)) * r;
			Vector2 b = Vector2(Math::sin(rb), Math::cos(rb)) * r;

			points.append(Vector3(a.x, a.y, 0));
			points.append(Vector3(b.x, b.y, 0));
		}

		PackedVector3Array collision_segments;

		for (int i = 0; i < 64; i++) {
			float ra = i * (Math_TAU / 64.0);
			float rb = (i + 1) * (Math_TAU / 64.0);
			Vector2 a = Vector2(Math::sin(ra), Math::cos(ra)) * r;
			Vector2 b = Vector2(Math::sin(rb), Math::cos(rb)) * r;

			collision_segments.append(Vector3(a.x, a.y, 0));
			collision_segments.append(Vector3(b.x, b.y, 0));
		}

		gizmo->add_lines(points, material);
		gizmo->add_collision_segments(collision_segments);
		PackedVector3Array handles;
		handles.append(Vector3(r, 0, 0));
		gizmo->add_handles(handles, handles_material, PackedInt32Array());
	}

	if (Object::cast_to<RectangleShape2D>(*s)) {
		Ref<RectangleShape2D> bs = s;
		PackedVector3Array lines;

		Vector2 size = bs->get_size();
		Vector2 hs = size * 0.5f;

		lines.append(Vector3(-hs.x, -hs.y, 0));
		lines.append(Vector3(-hs.x,  hs.y, 0));

		lines.append(Vector3(-hs.x,  hs.y, 0));
		lines.append(Vector3( hs.x,  hs.y, 0));

		lines.append(Vector3( hs.x,  hs.y, 0));
		lines.append(Vector3( hs.x, -hs.y, 0));

		lines.append(Vector3( hs.x, -hs.y, 0));
		lines.append(Vector3(-hs.x, -hs.y, 0));

		PackedVector3Array handles;

		handles.append(Vector3(hs.x, 0, 0));
		handles.append(Vector3(0, hs.y, 0));

		gizmo->add_lines(lines, material);
		gizmo->add_collision_segments(lines);
		gizmo->add_handles(handles, handles_material, PackedInt32Array());
	}

	if (Object::cast_to<CapsuleShape2D>(*s)) {
		Ref<CapsuleShape2D> cs2 = s;
		float radius = cs2->get_radius();
		float height = cs2->get_height();

		PackedVector3Array points;

		Vector3 d(0, height * 0.5 - radius, 0);
		for (int i = 0; i < 360; i++) {
			float ra = Math::deg_to_rad((float)i);
			float rb = Math::deg_to_rad((float)i + 1);
			Vector2 a = Vector2(Math::sin(ra), Math::cos(ra)) * radius;
			Vector2 b = Vector2(Math::sin(rb), Math::cos(rb)) * radius;
			Vector3 dud = i < 180 ? d : -d;

			if (i % 90 == 0) {
				points.append(Vector3(a.y, a.x, 0) + d);
				points.append(Vector3(a.y, a.x, 0) - d);
			}

			points.append(Vector3(a.y, a.x, 0) + dud);
			points.append(Vector3(b.y, b.x, 0) + dud);
		}

		gizmo->add_lines(points, material);

		PackedVector3Array collision_segments;

		for (int i = 0; i < 64; i++) {
			float ra = i * (Math_TAU / 64.0);
			float rb = (i + 1) * (Math_TAU / 64.0);
			Vector2 a = Vector2(Math::sin(ra), Math::cos(ra)) * radius;
			Vector2 b = Vector2(Math::sin(rb), Math::cos(rb)) * radius;
			Vector3 dud = i < 32 ? d : -d;

			if (i % 16 == 0) {
				collision_segments.append(Vector3(a.y, a.x, 0) + d);
				collision_segments.append(Vector3(a.y, a.x, 0) - d);
			}

			collision_segments.append(Vector3(a.y, a.x, 0) + dud);
			collision_segments.append(Vector3(b.y, b.x, 0) + dud);
		}

		gizmo->add_collision_segments(collision_segments);

		PackedVector3Array handles;
		handles.append(Vector3(cs2->get_radius(), 0, 0));
		handles.append(Vector3(0, cs2->get_height() * 0.5, 0));
		gizmo->add_handles(handles, handles_material, PackedInt32Array());
	}

	if (Object::cast_to<SeparationRayShape2D>(*s)) {
		Ref<SeparationRayShape2D> rs = s;
		float len = rs->get_length();

		PackedVector3Array lines;
		lines.append(Vector3());
		lines.append(Vector3(0, -len, 0));
		
		gizmo->add_lines(lines, material);
		gizmo->add_collision_segments(lines);
		PackedVector3Array handles;
		handles.push_back(Vector3(0, -len, 0));
		gizmo->add_handles(handles, handles_material, PackedInt32Array());
	}
}

void CollisionShapeSpatial2DGizmoPlugin::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_plugin", "plugin"), &CollisionShapeSpatial2DGizmoPlugin::set_plugin);
	ClassDB::bind_method(D_METHOD("get_plugin"), &CollisionShapeSpatial2DGizmoPlugin::get_plugin);
	ClassDB::bind_method(D_METHOD("initialize"), &CollisionShapeSpatial2DGizmoPlugin::initialize);
}