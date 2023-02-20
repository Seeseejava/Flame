#pragma once

#include "Runtime/ECS/System/System.h"
#include "Runtime/ECS/Scene/Scene.h"

namespace Flame
{
	class PythonScriptSystem : public System
	{
	public:
		PythonScriptSystem(Scene* scene) : System(scene) {}
		virtual ~PythonScriptSystem() = default;
	public:
		void OnUpdateRuntime(Timestep ts) override;
	};
}