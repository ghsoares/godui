#include "register_types.h"

#include "nodes/collision_object_spatial_2d.h"
#include "nodes/collision_shape_spatial_2d.h"
#include "nodes/physics_body_spatial_2d.h"
#include "gizmos/collision_shape_spatial_2d_gizmo_plugin.h"

#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

void initialize_physics_spatial_2d_module(ModuleInitializationLevel p_level) {
    switch (p_level) {
        case MODULE_INITIALIZATION_LEVEL_EDITOR: {
            ClassDB::register_class<CollisionShapeSpatial2DGizmoPlugin>();
        } break;
        case MODULE_INITIALIZATION_LEVEL_SCENE: {
            ClassDB::register_class<CollisionObjectSpatial2D>(true);
            ClassDB::register_class<CollisionShapeSpatial2D>();
            ClassDB::register_class<PhysicsBodySpatial2D>(true);
            ClassDB::register_class<StaticBodySpatial2D>();
            ClassDB::register_class<AnimatableBodySpatial2D>();
            ClassDB::register_class<CharacterBodySpatial2D>();
            ClassDB::register_class<RigidBodySpatial2D>();
        } break;
    }
}

void uninitialize_physics_spatial_2d_module(ModuleInitializationLevel p_level) {
    switch (p_level) {
        case MODULE_INITIALIZATION_LEVEL_EDITOR: {
            
        } break;
        case MODULE_INITIALIZATION_LEVEL_SCENE: {
            
        } break;
    }
}

extern "C" {
// Initialization.
GDExtensionBool GDE_EXPORT physics_spatial_2d_library_init(const GDExtensionInterface *p_interface, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
    godot::GDExtensionBinding::InitObject init_obj(p_interface, p_library, r_initialization);

    init_obj.register_initializer(initialize_physics_spatial_2d_module);
    init_obj.register_terminator(uninitialize_physics_spatial_2d_module);
    init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

    return init_obj.init();
}
}