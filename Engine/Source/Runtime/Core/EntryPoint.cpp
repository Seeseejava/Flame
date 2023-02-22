#include "flamepch.h"
#include "Runtime/Core/Application.h"

#include <pybind11/embed.h>

#ifdef FLAME_PLATFORM_WINDOWS

namespace Flame {

	// To be defined in CLIENT
	extern void MyAppInitialize(Application& app);

}

int main(int argc, char** argv)
{
	// https://github.com/pybind/pybind11/issues/3112
	// we should only have one interpreter in our program
	pybind11::scoped_interpreter guard{};

	Flame::MyAppInitialize(Flame::Application::GetInstance());
	Flame::Application::GetInstance().Run();
	Flame::Application::GetInstance().Clean();
}

#endif