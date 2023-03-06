#pragma once

#include "Runtime/Core/Timestep.h"
#include "Runtime/ECS/Scene/Scene.h"

namespace Flame
{
	class System
	{
	public:
		System() = delete;
		System(Scene* scene) : m_Scene(scene) {}
		virtual ~System() = default;
	public:
		virtual void OnUpdateEditor(Timestep ts, EditorCamera& camera) {};
		virtual void OnUpdateRuntime(Timestep ts) {}
		virtual void OnRuntimeStart() {}
		virtual void OnRuntimeStop() {}
	protected:
		Scene* m_Scene = nullptr;
	};
}