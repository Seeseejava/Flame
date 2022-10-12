#pragma once

#include <string>
#include <chrono>
#include <algorithm>
#include <fstream>

#include <thread>

// 目前存在问题：不能指定记录哪些帧的数据，而只能从头到尾执行
namespace Flame {

	struct ProfileResult
	{
		std::string Name;
		long long Start, End;
		uint32_t ThreadID;
	};

	struct InstrumentationSession
	{
		std::string Name;
	};

	// Instrumentor是个单例, 但是应该不是线程安全的单例
	class Instrumentor
	{
	private:
		InstrumentationSession* m_CurrentSession;
		std::ofstream m_OutputStream;
		int m_ProfileCount;
	public:
		Instrumentor()
			: m_CurrentSession(nullptr), m_ProfileCount(0)
		{
		}

		void BeginSession(const std::string& name, const std::string& filepath = "results.json")
		{
			m_OutputStream.open(filepath);
			WriteHeader();
			m_CurrentSession = new InstrumentationSession{ name }; // 注意InstrumentationSession是一个结构体
		}

		void EndSession()
		{
			WriteFooter();
			m_OutputStream.close();
			delete m_CurrentSession;
			m_CurrentSession = nullptr;
			m_ProfileCount = 0;
		}

		void WriteProfile(const ProfileResult& result)
		{
			if (m_ProfileCount++ > 0)
				m_OutputStream << ",";

			std::string name = result.Name;
			std::replace(name.begin(), name.end(), '"', '\'');

			m_OutputStream << "{";
			m_OutputStream << "\"cat\":\"function\",";
			m_OutputStream << "\"dur\":" << (result.End - result.Start) << ',';
			m_OutputStream << "\"name\":\"" << name << "\",";
			m_OutputStream << "\"ph\":\"X\",";
			m_OutputStream << "\"pid\":0,";
			m_OutputStream << "\"tid\":" << result.ThreadID << ",";
			m_OutputStream << "\"ts\":" << result.Start;
			m_OutputStream << "}";

			m_OutputStream.flush();
		}

		void WriteHeader()
		{
			m_OutputStream << "{\"otherData\": {},\"traceEvents\":[";
			m_OutputStream.flush();
		}

		void WriteFooter()
		{
			m_OutputStream << "]}";
			m_OutputStream.flush();
		}

		static Instrumentor& Get()
		{
			static Instrumentor instance; // 这一行只会执行一次
			return instance;
		}
	};

	class InstrumentationTimer
	{
	public:
		InstrumentationTimer(const char* name)
			: m_Name(name), m_Stopped(false)
		{
			m_StartTimepoint = std::chrono::high_resolution_clock::now();
		}

		~InstrumentationTimer()
		{
			if (!m_Stopped)
				Stop();
		}

		void Stop()
		{
			auto endTimepoint = std::chrono::high_resolution_clock::now();

			long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
			long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

			uint32_t threadID = std::hash<std::thread::id>{}(std::this_thread::get_id());//获取线程标识符的散列
			//这里为什么是用线程标识符的散列而不是直接使用线程标识符？
			/*使用get_id方法返回的是一个包装类型的std::thread::id对象，不可以直接强转成整型，也没有提供任何转换整型的接口
			，因此，一般使用std::cout将id值打印出来*/
			Instrumentor::Get().WriteProfile({ m_Name, start, end, threadID });

			m_Stopped = true;
		}
	private:
		const char* m_Name;
		std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimepoint;
		bool m_Stopped;
	};
}

#define FLAME_PROFILE 1
#if FLAME_PROFILE
	#define FLAME_PROFILE_BEGIN_SESSION(name, filepath) ::Flame::Instrumentor::Get().BeginSession(name, filepath)
	#define FLAME_PROFILE_END_SESSION() ::Flame::Instrumentor::Get().EndSession()
	#define FLAME_PROFILE_SCOPE(name) ::Flame::InstrumentationTimer timer##__LINE__(name);
	#define FLAME_PROFILE_FUNCTION() FLAME_PROFILE_SCOPE(__FUNCSIG__) //__funcsig__通常返回包含它的函数的签名
#else
	#define FLAME_PROFILE_BEGIN_SESSION(name, filepath)
	#define FLAME_PROFILE_END_SESSION()
	#define FLAME_PROFILE_SCOPE(name)
	#define FLAME_PROFILE_FUNCTION()
#endif