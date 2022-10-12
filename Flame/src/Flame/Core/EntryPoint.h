#pragma once

#ifdef FLAME_PLATFORM_WINDOWS

extern Flame::Application* Flame::CreateApplication();//ָ�봫�����Ա���������ڴ洦��

int main(int argc, char** argv)
{
	Flame::Log::Init();


	FLAME_PROFILE_BEGIN_SESSION("Startup", "FlameProfile-Startup.json");
	auto app = Flame::CreateApplication();
	FLAME_PROFILE_END_SESSION();

	FLAME_PROFILE_BEGIN_SESSION("Runtime", "FlameProfile-Runtime.json");
	app->Run();
	FLAME_PROFILE_END_SESSION();

	FLAME_PROFILE_BEGIN_SESSION("Shutdown", "FlameProfile-Shutdown.json");
	delete app;
	FLAME_PROFILE_END_SESSION();
}

#endif