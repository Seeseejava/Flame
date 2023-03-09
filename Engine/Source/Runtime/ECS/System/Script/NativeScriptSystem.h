#pragma once

#include "Runtime/ECS/System/System.h"
#include "Runtime/ECS/Scene/Scene.h"

namespace Flame
{
	class NativeScriptSystem : public System
	{
	public:
		NativeScriptSystem(Scene* Scene) : System(Scene) {}
		virtual ~NativeScriptSystem() = default;
	public:
		void OnUpdateRuntime(Timestep ts) override;
	};
}