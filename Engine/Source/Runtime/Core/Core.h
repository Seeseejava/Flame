#pragma once

#include <memory>

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

//如果要字符串化宏参数的扩展结果,必须使用两个级别的宏。
#define FLAME_XSTRING(x) FLAME_STRING(x)
#define FLAME_STRING(x)  #x
			//#define xstr(s) str(s)
			//#define str(s) #s
			//#define foo 4
			//str(foo)
			//→ "foo"
			//xstr(foo)
			//→ xstr(4)
			//→ str(4)
			//→ "4"

// #define FLAME_BIND_EVENT_FN(fn) std::bind(&fn , this, std::placeholders::_1)  两者等价且用lambda更佳
// Generic Lambda：当Lambda的参数列表里出现至少一个auto类型的参数，那么此Lambda就是一个Generic Lambda表达式
#define FLAME_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }
namespace Flame
{

	template<typename T>
	using Scope = std::unique_ptr<T>;

	template<typename T>
	using Ref = std::shared_ptr<T>;
	//为什么用 shared_ptr 而不是 unique_ptr:
	//因为我们的循环中要有这些东西的强引用，不能被销毁，否则直接crash；而 shared_ptr 通过它的引用计数做到了这一点。
}
