#pragma once

#ifdef FLAME_PLATFORM_WINDOWS
	#ifdef FLAME_BUILD_DLL
		#define FLAME_API _declspec(dllexport)
	#else 
		#define FLAME_API _declspec(dllimport)
	#endif
#else
	#error Flame only support windows!
#endif

#ifdef FLAME_DEBUG
	#define FLAME_ENABLE_ASSERTS
#endif

#ifdef FLAME_ENABLE_ASSERTS
	#define FLAME_ASSERT(x,...) {if(!(x)) { FLAME_ERROR("Assertion Failed: {0}",__VA_ARGS__); __debugbreak; }}
	#define FLAME_CORE_ASSERT(x,...) {if(!(x)) { FLAME_CORE_ERROR("Assertion Failed: {0}",__VA_ARGS__); __debugbreak; }}
#else
	#define FLAME_ASSERT(x, ...)
	#define FLAME_CORE_ASSERT(x, ...)
#endif 



#define BIT(x)  (1 << x)

#define FLAME_BIND_EVENT_FN(fn) std::bind(&fn , this, std::placeholders::_1)