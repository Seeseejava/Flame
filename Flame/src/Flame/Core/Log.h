#pragma once

#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h" //can log custom types like in our events

namespace Flame {

	class FLAME_API Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
		//�ǵ�C++Class�ڵ�static������Ҫ��������ж��壬����ֻ��������
	};

}

// Core log macros
#define FLAME_CORE_TRACE(...)     ::Flame::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define FLAME_CORE_INFO(...)      ::Flame::Log::GetCoreLogger()->info(__VA_ARGS__)
#define FLAME_CORE_WARN(...)      ::Flame::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define FLAME_CORE_ERROR(...)     ::Flame::Log::GetCoreLogger()->error(__VA_ARGS__)
#define FLAME_CORE_FATAL(...)     ::Flame::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define FLAME_TRACE(...)          ::Flame::Log::GetClientLogger()->trace(__VA_ARGS__)
#define FLAME_INFO(...)           ::Flame::Log::GetClientLogger()->info(__VA_ARGS__)
#define FLAME_WARN(...)           ::Flame::Log::GetClientLogger()->warn(__VA_ARGS__)
#define FLAME_ERROR(...)          ::Flame::Log::GetClientLogger()->error(__VA_ARGS__)
#define FLAME_FATAL(...)          ::Flame::Log::GetClientLogger()->critical(__VA_ARGS__)//video here is fatal
