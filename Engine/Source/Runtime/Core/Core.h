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
//__debugbreak() �൱��C#����Ե�Debug.break()�����Ե�ʱ�����ִ�е�������Զ���ϵ�
#else
	#define FLAME_ASSERT(x, ...)
	#define FLAME_CORE_ASSERT(x, ...)
#endif 



#define BIT(x)  (1 << x)

//���Ҫ�ַ��������������չ���,����ʹ����������ĺꡣ
#define FLAME_XSTRING(x) FLAME_STRING(x)
#define FLAME_STRING(x)  #x
			//#define xstr(s) str(s)
			//#define str(s) #s
			//#define foo 4
			//str(foo)
			//�� "foo"
			//xstr(foo)
			//�� xstr(4)
			//�� str(4)
			//�� "4"

// #define FLAME_BIND_EVENT_FN(fn) std::bind(&fn , this, std::placeholders::_1)  ���ߵȼ�����lambda����
// Generic Lambda����Lambda�Ĳ����б����������һ��auto���͵Ĳ�������ô��Lambda����һ��Generic Lambda���ʽ
#define FLAME_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }
namespace Flame
{

	template<typename T>
	using Scope = std::unique_ptr<T>;

	template<typename T>
	using Ref = std::shared_ptr<T>;
	//Ϊʲô�� shared_ptr ������ unique_ptr:
	//��Ϊ���ǵ�ѭ����Ҫ����Щ������ǿ���ã����ܱ����٣�����ֱ��crash���� shared_ptr ͨ���������ü�����������һ�㡣
}
