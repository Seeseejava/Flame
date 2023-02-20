#include "flamepch.h"

#include "Runtime/ECS/System/Script/PythonScriptSystem.h"

#include <pybind11/embed.h>

namespace Flame
{
    void PythonScriptSystem::OnUpdateRuntime(Timestep ts)
    {
        if (!std::getenv("PYTHONPATH") || !std::getenv("PYTHONHOME"))
        {
            return;
        }
        pybind11::scoped_interpreter guard{};
        pybind11::print("hello, world!");
    }
}