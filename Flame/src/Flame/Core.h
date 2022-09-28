#pragma once

#ifdef FLAME_PLATFORM_WINDOWS
#if FLAME_DYNAMIC_LINK
	#ifdef FLAME_BUILD_DLL
		#define FLAME_API _declspec(dllexport)
	#else 
		#define FLAME_API _declspec(dllimport)
	#endif
#else
	#define FLAME_API
#endif
#else
	#error Flame only support windows!
#endif

#ifdef FLAME_DEBUG
	#define FLAME_ENABLE_ASSERTS
#endif

#ifdef FLAME_ENABLE_ASSERTS
	#define FLAME_ASSERT(x, ...) { if(!(x)) { FLAME_ERROR("Assertion Failed: {0}",__VA_ARGS__); __debugbreak(); } }
	#define FLAME_CORE_ASSERT(x, ...) { if(!(x)) { FLAME_CORE_ERROR("Assertion Failed: {0}",__VA_ARGS__); __debugbreak(); }}
//__debugbreak() 相当于C#里调试的Debug.break()，调试的时候如果执行到这里会自动打断点
#else
	#define FLAME_ASSERT(x, ...)
	#define FLAME_CORE_ASSERT(x, ...)
#endif 



#define BIT(x)  (1 << x)

#define FLAME_BIND_EVENT_FN(fn) std::bind(&fn , this, std::placeholders::_1)