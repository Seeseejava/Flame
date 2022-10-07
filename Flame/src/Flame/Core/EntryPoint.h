#pragma once

#ifdef FLAME_PLATFORM_WINDOWS

extern Flame::Application* Flame::CreateApplication();//指针传过来以便引擎进行内存处理

int main(int argc, char** argv)
{
	Flame::Log::Init();

	FLAME_CORE_WARN("Initialzed Log!");
	int a = 5;
	FLAME_INFO("Hello! Var={0}", a);

	auto app = Flame::CreateApplication();
	app->Run();
	delete app;
}

#endif