#include "flamepch.h"
#include "Runtime/Core/Application.h"

#ifdef WITH_PYTHON
#include <pybind11/embed.h>
#endif

#ifdef FLAME_PLATFORM_WINDOWS

namespace Flame {

	// To be defined in CLIENT
	extern void MyAppInitialize(Application& app);

}

int main(int argc, char** argv)
{
#ifdef WITH_PYTHON
	// https://github.com/pybind/pybind11/issues/3112
	// we should only have one interpreter in our program
	pybind11::scoped_interpreter guard{};
#endif

	Flame::MyAppInitialize(Flame::Application::GetInstance());
	Flame::Application::GetInstance().Run();
	Flame::Application::GetInstance().Clean();
}

#endif