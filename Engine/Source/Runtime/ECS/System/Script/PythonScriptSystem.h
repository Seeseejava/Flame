#pragma once

#include "Runtime/ECS/System/System.h"
#include "Runtime/ECS/Scene/Scene.h"

namespace Flame
{
	class PythonScriptSystem : public System
	{
	public:
		PythonScriptSystem(Scene* scene);
		virtual ~PythonScriptSystem() = default;
	public:
		void OnUpdateRuntime(Timestep ts) override;
		void OnUpdateEditor(Timestep ts, EditorCamera& camera) override;
	private:
		bool bLoadPython = false;
	};
}