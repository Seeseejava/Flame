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