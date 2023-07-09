#include "register_types.h"

#include "ui.h"
#include "motion_ref.h"
#include "draw_ref.h"

#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

void initialize_godui_module(ModuleInitializationLevel p_level) {
    switch (p_level) {
        case MODULE_INITIALIZATION_LEVEL_SCENE: {
            ClassDB::register_class<UI>();
            ClassDB::register_class<MotionRef>();
            ClassDB::register_class<DrawRef>();
        } break;
    }
}

void uninitialize_godui_module(ModuleInitializationLevel p_level) {
    switch (p_level) {
        case MODULE_INITIALIZATION_LEVEL_SCENE: {
            
        } break;
    }
}

extern "C" {
// Initialization.
GDExtensionBool GDE_EXPORT godui_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
    godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

    init_obj.register_initializer(initialize_godui_module);
    init_obj.register_terminator(uninitialize_godui_module);
    init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

    return init_obj.init();
}
}