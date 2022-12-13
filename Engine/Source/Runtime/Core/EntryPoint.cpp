#include "flamepch.h"
#include "Runtime/Core/Application.h"

#ifdef FLAME_PLATFORM_WINDOWS

namespace Flame {

	// To be defined in CLIENT
	extern void MyAppInitialize(Application& app);

}

int main(int argc, char** argv)
{
	Flame::MyAppInitialize(Flame::Application::GetInstance());
	Flame::Application::GetInstance().Run();
	Flame::Application::GetInstance().Clean();
}

#endif