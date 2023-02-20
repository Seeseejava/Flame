#include "flamepch.h"

#include "Runtime/ECS/System/Script/PythonScriptSystem.h"

#include <pybind11/embed.h>

namespace py = pybind11;

namespace Flame
{
	PythonScriptSystem::PythonScriptSystem(Scene* scene)
		: System(scene)
	{
		if (std::getenv("PYTHONPATH") && std::getenv("PYTHONHOME"))
		{
			bLoadPython = true;
		}
	}

    void PythonScriptSystem::OnUpdateRuntime(Timestep ts)
    {
		if (!bLoadPython)
		{
			return;
		}
		py::scoped_interpreter guard{};
		auto testModule = py::module::import("Scripts.test");
		auto func = testModule.attr("OnUpdateRuntime");
		func();
    }

	void PythonScriptSystem::OnUpdateEditor(Timestep ts, EditorCamera& camera)
	{
		if (!bLoadPython)
		{
			return;
		}
		py::scoped_interpreter guard{};
		auto testModule = py::module::import("Scripts.test");
		auto func = testModule.attr("OnUpdateEditor");
		func();
	}
}