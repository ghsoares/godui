#ifndef COLLISION_SHAPE_SPATIAL_2D_GIZMO_PLUGIN_H
#define COLLISION_SHAPE_SPATIAL_2D_GIZMO_PLUGIN_H

#include <godot_cpp/classes/editor_node3d_gizmo_plugin.hpp>
#include <godot_cpp/classes/editor_plugin.hpp>
#include <godot_cpp/classes/camera3d.hpp>
#include "../nodes/collision_shape_spatial_2d.h"

namespace godot {

class CollisionShapeSpatial2DGizmoPlugin : public EditorNode3DGizmoPlugin {
	GDCLASS(CollisionShapeSpatial2DGizmoPlugin, EditorNode3DGizmoPlugin);

	EditorPlugin* plugin;
protected:
	static void _bind_methods();

public:
	_FORCE_INLINE_ void set_plugin(EditorPlugin* p_plugin) {plugin = p_plugin;}
	_FORCE_INLINE_ EditorPlugin* get_plugin() const {return plugin;}

	void initialize();

	bool _has_gizmo(Node3D *p_spatial) const override;
	String _get_gizmo_name() const override;
	int _get_priority() const override;
	void _redraw(const Ref<EditorNode3DGizmo> &p_gizmo) override;

	String _get_handle_name(const Ref<EditorNode3DGizmo> &p_gizmo, int p_id, bool p_secondary) const override;
	Variant _get_handle_value(const Ref<EditorNode3DGizmo> &p_gizmo, int p_id, bool p_secondary) const override;
	void _set_handle(const Ref<EditorNode3DGizmo> &p_gizmo, int32_t p_id, bool p_secondary, Camera3D *p_camera, const Vector2 &p_point) override;
	void _commit_handle(const Ref<EditorNode3DGizmo> &p_gizmo, int p_id, bool p_secondary, const Variant &p_restore, bool p_cancel = false) override;

	CollisionShapeSpatial2DGizmoPlugin();
};

}

#endif // COLLISION_SHAPE_SPATIAL_2D_GIZMO_PLUGIN_H