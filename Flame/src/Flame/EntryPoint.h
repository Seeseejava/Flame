#pragma once

#ifdef FLAME_PLATFORM_WINDOWS

extern Flame::Application* Flame::CreateApplication();

int main(int argc, char** argv)
{
	
	auto app = Flame::CreateApplication();
	app->Run();
	delete app;
}

#endif