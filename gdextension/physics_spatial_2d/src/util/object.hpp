#ifndef UTIL_OBJECT_H
#define UTIL_OBJECT_H

#include <godot_cpp/classes/object.hpp>

namespace godot {

class ObjectUtils {
public:
	template<class T>
	static T *cast_to(Object *p_object);
	template<class T>
	static const T *cast_to(const Object *p_object);
};

template <class T>
T *ObjectUtils::cast_to(Object *p_object) {
	if (p_object == nullptr) {
		return nullptr;
	}
	StringName class_name = T::get_class_static();
	GDExtensionObjectPtr casted = internal::gde_interface->object_cast_to(p_object->_owner, internal::gde_interface->classdb_get_class_tag(class_name._native_ptr()));
	if (casted == nullptr) {
		return nullptr;
	}
	return dynamic_cast<T *>((Object *)internal::gde_interface->object_get_instance_binding(casted, internal::token, &T::___binding_callbacks));
	// return Object::cast_to<T>(p_object);
}

template <class T>
const T *ObjectUtils::cast_to(const Object *p_object) {
	if (p_object == nullptr) {
		return nullptr;
	}
	StringName class_name = T::get_class_static();
	GDExtensionObjectPtr casted = internal::gde_interface->object_cast_to(p_object->_owner, internal::gde_interface->classdb_get_class_tag(class_name._native_ptr()));
	if (casted == nullptr) {
		return nullptr;
	}
	return dynamic_cast<const T *>((Object *)internal::gde_interface->object_get_instance_binding(casted, internal::token, &T::___binding_callbacks));
	// return Object::cast_to<T>(p_object);
}

}

#endif // UTIL_OBJECT_H