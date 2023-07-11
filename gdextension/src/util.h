#ifndef GODUI_UTIL_H
#define GODUI_UTIL_H

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/templates/hash_map.hpp>
#include <godot_cpp/variant/callable.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#ifndef __FUNCTION_NAME__
    #ifdef WIN32   //WINDOWS
        #define __FUNCTION_NAME__   __FUNCTION__  
    #else          //*NIX
        #define __FUNCTION_NAME__   __func__ 
    #endif
#endif

#define GP_STR(x) #x
#define GP_TOSTR(x) GP_STR(x)
#define DBG() UtilityFunctions::print(vformat("%s:%s:%s", __FILE__, __FUNCTION_NAME__, __LINE__));

namespace godot {

template<class... Args>
Variant Callable::call(const Args&... args) const {
	return this->callv(Array::make(args...));
}

}

#endif // GODUI_UTIL_H