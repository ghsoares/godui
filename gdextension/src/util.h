#ifndef GODUI_UTIL_H
#define GODUI_UTIL_H

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/templates/hash_map.hpp>
#include <godot_cpp/variant/callable.hpp>

namespace godot {

template<class... Args>
Variant Callable::call(const Args&... args) const {
	return this->callv(Array::make(args...));
}

}

#endif // GODUI_UTIL_H